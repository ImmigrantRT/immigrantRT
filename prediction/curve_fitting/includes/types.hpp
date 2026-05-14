// Custom aliases for longer types

#ifndef TYPES_HPP
#define TYPES_HPP

#include <vector>
#include <utility>

// for a pair of (t, x) values; t = time, x = resource usage/ availability
using DataPoint = std::pair<double, double>;

// for storing cpu data or mem data or both
using Data = std::vector<DataPoint>;
using FullData = std::pair<Data, Data>;

using Matrix = std::vector<std::vector<double>>;

// for predictions + the corresponding margins of error
using PredsWErrors = std::pair<std::vector<double>, std::vector<double>>;

#endif
