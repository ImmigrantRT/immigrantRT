#include "../includes/candidate.hpp"

std::vector<Node> GetCandidates(
    std::vector<Node> nodes, 
    Container container,
    std::function<bool(Node, Container)> HasEnoughResources
) {

    nodes.erase(
        std::remove_if(
            nodes.begin(), nodes.end(), 
            [&container, HasEnoughResources](Node node) {
                // resource check: 
                // - always needed for ranking
                // - not needed for validation if no new data is available from the remote nodes 
                bool resource_checks = HasEnoughResources ? HasEnoughResources(node, container) : true;
                bool health_check = node.GetHealthScore() >= 0.75;
                return !resource_checks || !health_check;
            }
        ), 
        nodes.end()
    );

    return nodes;
}

void RankCandidates(std::vector<Node>& candidates) {
    std::sort(
        candidates.begin(), candidates.end(),
        [](Node n1, Node n2) {
            return n1.GetThroughput() > n2.GetThroughput(); // descending order of throughput
        }
    );
}
