// Custom aliases for longer types

#ifndef TYPES_HPP
#define TYPES_HPP

#include <vector>
#include <map>
#include <boost/variant.hpp>

#include "datapoint.hpp"

// for the tokens in each line of an input file
using Tokens = std::vector<boost::variant<std::string, double>>;

// for the resource requirements of a container
// variants: (1) reqs on one node, (2) reqs on multiple nodes
using ContainerReqs = boost::variant<DataPoint1, std::map<std::string, DataPoint1>>;

#endif
