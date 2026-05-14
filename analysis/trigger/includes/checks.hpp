#ifndef TRIGGER_CHECKS_HPP
#define TRIGGER_CHECKS_HPP

#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <boost/variant.hpp>

#include "../../shared/includes/datapoint.hpp"
#include "../../shared/includes/types.hpp"
#include "../../shared/includes/node.hpp"
#include "../../shared/includes/container.hpp"

#define QOS_THRESHOLD 10

bool HasEnoughResources(Node, std::vector<Container>);
bool HasGoodQoS(std::vector<std::pair<std::string, int>>&);

#endif
