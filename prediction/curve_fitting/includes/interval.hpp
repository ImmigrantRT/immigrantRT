#ifndef INTERVAL_HPP
#define INTERVAL_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <boost/math/distributions/students_t.hpp>

#include "types.hpp"
#include "model.hpp"

double ComputeResidualVariance(Data, std::vector<double>);
double ComputeLeverage(std::vector<double>, double);
double GetCriticalValueTDistribution(size_t, size_t);
std::vector<double> ComputeMarginOfError(Data, std::vector<double>, int);

#endif