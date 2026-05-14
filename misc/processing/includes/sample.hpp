#ifndef SAMPLE_HPP
#define SAMPLE_HPP

#include <string>
#include <sstream>
#include <vector>

class Sample {
public:
	Sample(double, std::string);
	Sample(double, double, double);
	double GetTimeElapsed();
	double GetCpu();
	double GetMem();
private:
	double time_elapsed_;
	double cpu_;
	double mem_;
};

std::vector<Sample> interpolation(std::vector<Sample>&, Sample);

#endif