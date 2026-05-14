#include <iostream>
#include <vector>

#include "../../shared/includes/utilities.hpp"
#include "../../shared/includes/node.hpp"
#include "../../shared/includes/container.hpp"
#include "../../shared/includes/candidate.hpp"

#include "../includes/checks.hpp"

int main(int argc, char* argv[]) {
    // argv:
    // - folder
    // - container id
    // - 0 or 1 (whether new data available from remote nodes or not)

    if (argc != 4) {
        std::cout << "3 arguments needed" << std::endl;
        return 1;
    }

    std::string folder(argv[1]); // path to the data folder
    std::string container_id(argv[2]);
    bool read_preds = std::atoi(argv[3]) == 1; // 1 = read predictions 0 = not needed

    std::string filename = folder + "/containers/" + container_id + "/rank.txt"; // path to rank.txt

    // create an object of the Container class
    Container container(folder, container_id, 0); // criticality is not relevant

    // get IP addresses of all candidate nodes for the container
    std::vector<std::string> candidates_ip = ReadNodeIP(filename);

    // create objects of the Node class
    std::vector<Node> candidates;

    for (std::string ip : candidates_ip) {
        Node candidate(folder, ip, false, read_preds); // false = use predictions
        candidates.push_back(candidate);
    }

    // get candidate nodes
    if (read_preds) {
        candidates = GetCandidates(candidates, container, HasEnoughResources);
    } else {
        candidates = GetCandidates(candidates, container);
    }

    // sort the candidates in descending order based on throughput
    RankCandidates(candidates);

    candidates_ip.clear();
    for (Node& candidate : candidates) {
        candidates_ip.push_back(candidate.GetIP());
    }

    WriteFileIfNeeded(filename, candidates_ip);

    return 0;
}
