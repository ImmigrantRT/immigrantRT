#ifndef WEIGHTS_HPP
#define WEIGHTS_HPP

#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>

void normalize(std::vector<double>&);
std::vector<double> equal_weights(int);
std::vector<double> geometric_progression_weights(int);
std::vector<double> sigmoid_weights(int);

#endif
