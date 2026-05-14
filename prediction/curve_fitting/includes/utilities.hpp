#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <sstream>
#include <iomanip>

#include "types.hpp"

const std::string GetFilename(const std::string, const std::string);
std::string ExtractString(std::string&, std::string);
FullData ReadFile(const std::string);
void PrintCoeffs(double*, int);
void PrintResults(double*, int, double, double);
void WriteToFile(const std::string, PredsWErrors, PredsWErrors);

#endif