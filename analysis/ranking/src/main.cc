#include <iostream>
#include <vector>

#include "../../shared/includes/utilities.hpp"
#include "../../shared/includes/node.hpp"
#include "../../shared/includes/container.hpp"
#include "../../shared/includes/candidate.hpp"

#include "../includes/checks.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "1 argument needed" << std::endl;
        return 1;
    }

    std::string folder(argv[1]); // path to the data folder

    // get IP addresses of all remote nodes
    std::vector<std::string> node_ip = ReadNodeIP(folder + "/nodes.txt");

    // create objects of the Node class
    std::vector<Node> nodes;

    for (std::string ip : node_ip) {
        Node node(folder, ip, true); // true = use forecasts
        nodes.push_back(node);
    }

    // get container IDs and criticaity
    std::map<std::string, int> container_info = ReadContainerInfo(folder);

    for (const auto& c : container_info) {
        Container container(folder, c.first, c.second);

        // get candidate nodes
        std::vector<Node> candidates = GetCandidates(nodes, container, HasEnoughResources);

        // sort the candidate nodes in descending order based on throughput
        RankCandidates(candidates);

        std::vector<std::string> candidates_ip;
        for (Node& candidate : candidates) {
            candidates_ip.push_back(candidate.GetIP());
        }

        std::string filename = folder + "/containers/" + container.GetID() + "/rank.txt";
        WriteFileIfNeeded(filename, candidates_ip);
    }

    return 0;
}
