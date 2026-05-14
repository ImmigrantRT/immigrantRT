#include <iostream>
#include <vector>

#include "time.hpp"
#include "utilities.hpp"
#include "sample.hpp"

#include<iostream>
using namespace std;

int main(int argc, char** argv) {
	if (argc != 3) {
		std::cout << "Expected 2 arguments - folder, reference time" << std::endl;
		return 1;
	}
	
	std::vector<std::string> timestamps;
	std::vector<std::string> data;
	ReadData(argv[1], timestamps, data);

	std::string prev = ReadLastProcessedSample(argv[1]);
	double t_prev = std::stod(prev.substr(0, prev.find(",")));
	std::string data_prev = prev.substr(prev.find(",") + 1, prev.length());
	Sample prev_sample(t_prev, data_prev);
	
	// compute time elapsed from the first reading
	Time t_ref(argv[2]);

	std::vector<Sample> samples;
	for (int i = 0; i < timestamps.size(); i++) {
		Time t(timestamps[i]);
		
		double time_elapsed = t - t_ref;
		time_elapsed = std::round(time_elapsed);

		if (time_elapsed > t_prev) {
			Sample sample(time_elapsed, data[i]);
			samples.push_back(sample);
		}
	}

	std::vector<Sample> processed_samples = interpolation(samples, prev_sample);
	WriteFile(argv[1], processed_samples);
	
	return 0;  
}
