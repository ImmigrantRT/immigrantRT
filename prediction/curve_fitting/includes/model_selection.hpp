#ifndef MODEL_SELECTION_HPP
#define MODEL_SELECTION_HPP

#include <iostream>
#include <vector>
#include <limits>
#include <math.h>

#include "types.hpp"
#include "model.hpp"
#include "utilities.hpp"

std::vector<double> GetPredictions(Data, int);
std::vector<double> FindBestFitCurve(Data, int);

#endif