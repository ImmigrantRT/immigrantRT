#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "types.hpp"
#include "utilities.hpp"

class Container {
public:
    Container(std::string, std::string, int, std::string = "");

    std::string GetID();
    int GetCritLevel();
    ContainerReqs GetRequirements();

private:
    // to read requirements on all nodes
    std::map<std::string, DataPoint1> ReadRequirements(std::string); 

    // to get the requirements on a single node
    void ReadRequirements(std::string, std::string); 

    std::string id_;
    int crit_level_;
    ContainerReqs requirements_;
};

#endif
