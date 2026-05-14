#include "../includes/container.hpp"

Container::Container(std::string folder, std::string id, int crit, std::string node_ip) {
    id_ = id;
    crit_level_ = crit;

    if (node_ip.empty()) { // for ranking, validation
        requirements_ = ReadRequirements(folder);
    } else { // for trigger
        ReadRequirements(folder, node_ip);
    }
}

std::string Container::GetID() { return id_; }

int Container::GetCritLevel() { return crit_level_; }

ContainerReqs Container::GetRequirements() { return requirements_; }

std::map<std::string, DataPoint1> Container::ReadRequirements(std::string folder) {
    // to store container requirements on all nodes
    std::map<std::string, DataPoint1> requirements;

    std::ifstream file(folder + "/containers/" + id_ + "/requirements.txt");
    
    std::string line;
    while (getline(file, line)) {
        Tokens tokens = SplitString(line);

        requirements[boost::get<std::string>(tokens[0])] = DataPoint1(
            boost::get<double>(tokens[1]), boost::get<double>(tokens[2]
        ));
    }

    file.close();
    return requirements;
}

void Container::ReadRequirements(std::string folder, std::string node_ip) {
    std::map<std::string, DataPoint1> requirements = ReadRequirements(folder); 

    std::map<std::string, DataPoint1>::const_iterator it = requirements.find(node_ip);

    if (it != requirements.end()) {
        requirements_ = it->second;
    } else {
        requirements_ = DataPoint1();
    }
}
