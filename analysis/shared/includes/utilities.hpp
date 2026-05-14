#ifndef SHARED_UTILITIES_HPP
#define SHARED_UTILITIES_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <sys/sysinfo.h>

#include "types.hpp"

bool IsNumber(const std::string&);
Tokens SplitString(const std::string&, char = ',');
double GetTotalMemory();
std::vector<std::string> ReadNodeIP(const std::string&);
std::map<std::string, int> ReadContainerInfo(const std::string&);
void WriteFileIfNeeded(const std::string&, const std::vector<std::string>&);

#endif
