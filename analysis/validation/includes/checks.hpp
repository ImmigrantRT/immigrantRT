#ifndef VALIDATION_CHECKS_HPP
#define VALIDATION_CHECKS_HPP

#include <vector>
#include <map>
#include <string>
#include <boost/variant.hpp>

#include "../../shared/includes/datapoint.hpp"
#include "../../shared/includes/types.hpp"
#include "../../shared/includes/node.hpp"
#include "../../shared/includes/container.hpp"

bool HasEnoughResources(Node, Container);

#endif
