#ifndef SELECTION_HPP
#define SELECTION_HPP

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include "../../shared/includes/datapoint.hpp"
#include "../../shared/includes/container.hpp"

std::string FindContainerQoS(std::vector<std::pair<std::string, int>>);

bool CompareCPUMem(
    const std::pair<std::string, DataPoint1>&, 
    const std::pair<std::string, DataPoint1>&
);

std::string FindContainerCriticality(std::vector<Container>);

std::string SelectContainer(
    std::vector<Container>, 
    std::vector<std::pair<std::string, int>>, 
    bool, 
    bool
);

#endif
