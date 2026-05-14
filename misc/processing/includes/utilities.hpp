#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <fstream>
#include <vector>
#include <string.h>

#include "sample.hpp"

void ReadData(std::string, std::vector<std::string>&, std::vector<std::string>&);
std::string ReadLastProcessedSample(std::string);
void WriteFile(std::string, std::vector<Sample>);

#endif