#include "stats.hpp"

float ToKB(float mem, const std::string& unit) {
	if (unit == "KiB") {
		return mem * 1.024;
	} else if (unit == "MiB") {
		return mem * 1.024 * 1024;
	} else if (unit == "GiB") {
		return mem * 1.024 * 1024 * 1024;
	}
	return -1; // default
}

std::string ProcessStats(const std::string& stats, int cpus) {
	// example: input = "21.48% 36.281MiB / 31.34GiB"
	
	std::istringstream iss(stats);

	// split the input string by spaces
	std::string substr;
	std::getline(iss, substr, ' ');

	std::string cpu_usage = substr.substr(0, substr.size() - 1); // remove % symbol
	// std::cout << "CPU usage: " << cpu_usage <<std::endl;
	float cpu_usage_f = std::stof(cpu_usage)/float(cpus);
	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << cpu_usage_f;
	cpu_usage = ss.str();

	std::getline(iss, substr, ' ');

	std::regex pattern(R"(([0-9.]+)([a-zA-Z]+))"); // alphanumeric
	std::smatch match;
	std::regex_search(substr, match, pattern);

	float value = std::stof(match.str(1));
	std::string unit = match.str(2);

	// convert to kilobytes
	float mem_usage = ToKB(value, unit);
	// std::cout << "Mem usage: " << mem_usage << " KB" <<std::endl;

	return cpu_usage + ',' + std::to_string(mem_usage);
}

std::string GetPodmanStats(const std::string& id, int cpus) {
	// use the Podman API to get the CPU and memory usage of the container
	std::string cmd = "podman stats --no-stream --format '{{.CPUPerc}} {{.MemUsageBytes}}' " + id;

	std::string stats = ExecuteCommand(cmd);
	// std::cout << "Stats: " << stats << std::endl;

	stats = ProcessStats(stats, cpus);

	return stats;
}
