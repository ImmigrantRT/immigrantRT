#include "../includes/checks.hpp"

bool HasEnoughResources(Node node, Container container) {
    std::string node_ip = node.GetIP();

    ContainerReqs c_reqs = container.GetRequirements();
    std::map<std::string, DataPoint1>* reqs = boost::get<std::map<std::string, DataPoint1>>(&c_reqs);

    std::vector<DataPoint1> node_forecasts = node.GetForecasts();

    // check the condition for each timestamp within the forecast horizon
    for (int i = 0; i < node_forecasts.size(); i++) {
        bool cpu_check = node_forecasts[i].cpu >= reqs->at(node_ip).cpu;
        bool mem_check = node_forecasts[i].mem >= reqs->at(node_ip).mem;
        
        if (!cpu_check || !mem_check) {
            return false;
        }
    }

    return true;
}
