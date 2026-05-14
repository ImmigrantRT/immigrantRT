#include "sample.hpp"

Sample::Sample(double time_elapsed, std::string data) {
	time_elapsed_ = time_elapsed;
	
	std::stringstream ss(data);
	std::vector<std::string> split;
	while(ss.good()) {
		std::string substr;
		getline(ss, substr, ',');
		split.push_back(substr);
	}

	cpu_ = std::stod(split[0]);
	mem_ = std::stod(split[1]);
}

Sample::Sample(double time_elapsed, double cpu, double mem) {
	time_elapsed_ = time_elapsed;
	cpu_ = cpu;
	mem_ = mem;
}

double Sample::GetTimeElapsed() {
	return time_elapsed_;
}

double Sample::GetCpu() {
	return cpu_;
}

double Sample::GetMem() {
	return mem_;
}

double get_interpolation_value(double x1, double y1, double x2, double y2, double x) {
	double m = (y2 - y1) / (x2 - x1);
	double y = y1 + m * (x - x1);
	return y;
}

std::vector<Sample> interpolation(std::vector<Sample> &samples, Sample prev_sample) {
	std::vector<Sample> processed_samples;

	double t_prev = prev_sample.GetTimeElapsed();
	double cpu_prev = prev_sample.GetCpu();
	double mem_prev = prev_sample.GetMem();

	int i = 0;

	if (t_prev == -1) {
		// add first sample without any modification
		processed_samples.push_back(samples[0]);
		t_prev = samples[0].GetTimeElapsed();
		cpu_prev = samples[0].GetCpu();
		mem_prev = samples[0].GetMem();
		i++;
	} 

	for (; i < samples.size(); i++) {
		double t_curr = samples[i].GetTimeElapsed();
		double cpu_curr = samples[i].GetCpu();
		double mem_curr = samples[i].GetMem();

		if (t_curr != t_prev + 1) { 
			// assuming frequency = 1 sample/ sec
			if (t_curr - t_prev == 2) { // to handle gaps due to rounding off the time elapsed
				double cpu_new = get_interpolation_value(t_prev, cpu_prev, t_curr, cpu_curr, t_prev + 1);
				double mem_new = get_interpolation_value(t_prev, mem_prev, t_curr, mem_curr, t_prev + 1);
				Sample sample(t_prev + 1, cpu_new, mem_new);
				processed_samples.push_back(sample);
			} else {
				for (int j = 1; j < t_curr - t_prev; j++) {
					Sample sample(t_prev + j, 0, 0);
					processed_samples.push_back(sample);
				}
			}
		} 

		processed_samples.push_back(samples[i]); // push back sample i

		t_prev = t_curr;
		cpu_prev = cpu_curr;
		mem_prev = mem_curr;
	}

	return processed_samples;
}