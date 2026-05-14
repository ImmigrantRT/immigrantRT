#ifndef STATS_HPP
#define STATS_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <sstream>  // Required for stringstream
#include <iomanip>  // Required for setprecision

#include "command.hpp"

float ToKB(float, const std::string&);
std::string ProcessStats(const std::string&, int cpus);
std::string GetPodmanStats(const std::string&, int cpus);

#endif
