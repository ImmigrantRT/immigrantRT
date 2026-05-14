#include "utilities.hpp"

void ReadData(std::string folder, std::vector<std::string> &t, std::vector<std::string> &data) {
	std::string filename = folder + '/' + "stats.txt";
	std::ifstream infile(filename);

	std::string line;
	int s = 0;
	while (getline(infile, line)) {
		size_t idx = line.find("]");
		t.push_back(line.substr(0, idx + 1));
		data.push_back(line.substr(idx + 2));
	}
}

std::string ReadLastProcessedSample(std::string folder) {
	// returns the last non-empty line

	std::string filename = folder + '/' + "stats_processed.csv";
	std::ifstream infile(filename);

	// handle empty file
	if (infile.peek() == std::ifstream::traits_type::eof()) {
		return "-1,0,0";
	}

	std::string prev_line;
	std::string line;
	while (getline(infile, line)) {
		if (line.compare("") != 0) {
			prev_line = line;
		}
	}

	return prev_line;	
}

void WriteFile(std::string folder, std::vector<Sample> samples) {
	std::string filename = folder + '/' + "stats_processed.csv";

	std::ofstream outfile;
	outfile.open(filename, std::ios::app);

	for (int i = 0; i < samples.size(); i++) {
		outfile << std::to_string(samples[i].GetTimeElapsed()) << ",";
		outfile << std::to_string(samples[i].GetCpu()) << ",";
		outfile << std::to_string(samples[i].GetMem()) << std::endl;
	}
}