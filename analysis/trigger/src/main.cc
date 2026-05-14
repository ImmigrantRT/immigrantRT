#include <iostream>
#include <vector>
#include <map>
#include <boost/variant.hpp>

#include "../../shared/includes/utilities.hpp"
#include "../../shared/includes/node.hpp"
#include "../../shared/includes/container.hpp"

#include "../includes/checks.hpp"
#include "../includes/selection.hpp"

int main(int argc, char* argv[]) {
    /* 
     * Command line arguments: 
     * 1. path to the data folder
     * 2. ip address of current node
     */

    if (argc != 3) {
        std::cout << "2 arguments needed" << std::endl;
        return 1;
    }

    std::string folder(argv[1]);
    std::string host_ip(argv[2]);

    // create an object of the Node class for the current node
    Node node(folder, "self", false); // false = use predictions

    // get container IDs and criticaity
    std::map<std::string, int> container_info = ReadContainerInfo(folder);

    // create objects of the Container class
    std::vector<Container> containers;

    for (const auto& c : container_info) {
        Container container(folder, c.first, c.second, host_ip);
        containers.push_back(container);
    }

    // CPU + mem checks
    bool resource_check = HasEnoughResources(node, containers);
    // std::cout << "CPU & Mem available? " << (resource_check ? "yes" : "no") << std::endl;

    std::vector<std::pair<std::string, int>> lowest_qos_freq = node.GetLowestQoSFreq();

    // QoS check
    bool qos_check = HasGoodQoS(lowest_qos_freq);
    // std::cout << "QoS check? " << (qos_check ? "passed" : "failed") << std::endl;

    // migration decision + node selection
    std::string container_selected = SelectContainer(containers, lowest_qos_freq, resource_check, qos_check);
    std::cout << container_selected << std::endl;

    return 0;
}
