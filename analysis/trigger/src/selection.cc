#include "../includes/selection.hpp"

std::string FindContainerQoS(std::vector<std::pair<std::string, int>> lowest_qos_freq) {
    // vector is already sorted ==> pick first one

    return lowest_qos_freq.front().first;
}

bool CompareCPUMem(
    const std::pair<std::string, DataPoint1>& r1, 
    const std::pair<std::string, DataPoint1>& r2) {

    // compare by CPU required
    if (r1.second.cpu != r2.second.cpu) {
        return r1.second.cpu > r2.second.cpu;
    }

    // if tied, compare by mem required
    return r1.second.mem >= r2.second.mem;
}

std::string FindContainerCriticality(std::vector<Container> containers) {

    for (int crit = 1; crit <= 4; crit++) {
        // 1 = critical, 2 = high, 3 = medium, 4 = low

        std::vector<std::pair<std::string, DataPoint1>> filtered;
        for (Container c : containers) {
            if (c.GetCritLevel() == crit) {
                ContainerReqs c_reqs = c.GetRequirements();
                DataPoint1* reqs = boost::get<DataPoint1>(&c_reqs);

                filtered.push_back({c.GetID(), *reqs});
            }
        }

        // sort them in descending order of CPU required, then mem required
        std::sort(filtered.begin(), filtered.end(), CompareCPUMem);

        if (!filtered.empty()) {
            return filtered.front().first;
        }
    } 

    return "";   
}

std::string SelectContainer(
    std::vector<Container> containers, 
    std::vector<std::pair<std::string, int>> lowest_qos_freq, 
    bool resource_check, 
    bool qos_check) {
    
    if (!qos_check) {
        // select the container that appears most frequently in QoS.csv
        return FindContainerQoS(lowest_qos_freq); 
    } else {
        if (!resource_check) {
            // selection based on criticality, cpu, mem required (in that order)
            return FindContainerCriticality(containers);
        }
        // else: no migration
    }

    return "";
}
