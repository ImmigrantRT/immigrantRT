#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <cmath>
#include <algorithm>

#include "types.hpp"
#include "matrix.hpp"

Matrix CreateAugmentedMatrix(Data&, int);
std::vector<double> GaussJordanElimination(Matrix&);
std::vector<double> Fit(Data&, int);
double Predict(Data, std::vector<double>, int);

#endif