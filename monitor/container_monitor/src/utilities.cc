#include "utilities.hpp"

void WriteToFile(
    const std::string& filename, 
    const std::string& ts, const std::string& stats
) {
	std::ofstream outfile(filename, std::ios::app);
	if (!outfile) {
		std::cerr << "Error: Cannot open file!" << std::endl;
		return;
	}

	outfile << ts << ' ' << stats << '\n';

	outfile.close();
}
