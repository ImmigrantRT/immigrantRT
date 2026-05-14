#include "../includes/checks.hpp"

bool HasEnoughResources(Node node, std::vector<Container> containers) {
    std::vector<DataPoint2> node_preds = node.GetPredictions();

    double total_cpu_req = 0.0;
    double total_mem_req = 0.0;

    for (Container c : containers) {
        ContainerReqs c_reqs = c.GetRequirements();

        DataPoint1* reqs = boost::get<DataPoint1>(&c_reqs);

        total_cpu_req += reqs->cpu;
        total_mem_req += reqs->mem;
    }

    // check the condition for each timestamp within the prediction horizon
    for (int i = 0; i < node_preds.size(); i++) {
        bool cpu_check = node_preds[i].values.cpu + node_preds[i].intervals.cpu + total_cpu_req <= 80;
        bool mem_check = node_preds[i].values.mem + node_preds[i].intervals.mem + total_mem_req <= GetTotalMemory();
        
        if (!cpu_check || !mem_check) {
            return false;
        }
    }

    return true;
}

bool HasGoodQoS(std::vector<std::pair<std::string, int>>& freq) {
    std::sort(
        freq.begin(), freq.end(), 
        [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
            return a.second > b.second; // descending order
        }
    );

    if (!freq.empty()) {
        return freq.front().second < QOS_THRESHOLD;
    }

    return true;
}
