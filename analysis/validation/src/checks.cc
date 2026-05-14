#include "../includes/checks.hpp"

bool HasEnoughResources(Node node, Container container) {
    std::string node_ip = node.GetIP();

    ContainerReqs c_reqs = container.GetRequirements();
    std::map<std::string, DataPoint1>* reqs = boost::get<std::map<std::string, DataPoint1>>(&c_reqs);

    std::vector<DataPoint2> node_preds = node.GetPredictions();

    // check the condition for each timestamp within the prediction horizon
    for (int i = 0; i < node_preds.size(); i++) {
        bool cpu_check = (node_preds[i].values.cpu - node_preds[i].intervals.cpu) >= reqs->at(node_ip).cpu;
        bool mem_check = (node_preds[i].values.mem - node_preds[i].intervals.mem) >= reqs->at(node_ip).mem;
        
        if (!cpu_check || !mem_check) {
            return false;
        }
    }

    return true;
}
