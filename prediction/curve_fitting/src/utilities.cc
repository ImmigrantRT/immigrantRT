#include "utilities.hpp"

const std::string GetFilename(const std::string folder, const std::string file) {
	return (folder.back() == '/') ? folder + file : folder + '/' + file;
}

std::string ExtractString(std::string& s, std::string delimiter) {
	std::string substr = s.substr(0, s.find(","));
	s.erase(0, s.find(",") + 1);
	return substr;
}

FullData ReadFile(const std::string filename) {
	Data cpu_data, mem_data;

	std::ifstream infile(filename);
	std::string line;

	while (getline(infile, line)) {
		std::string t = ExtractString(line, ",");
		std::string x1 = ExtractString(line, ","); // cpu
		std::string x2 = ExtractString(line, ","); // mem

		cpu_data.push_back(std::make_pair(std::stod(t), std::stod(x1)));
		mem_data.push_back(std::make_pair(std::stod(t), std::stod(x2)));
	}

	return std::make_pair(cpu_data, mem_data);
}

void PrintCoeffs(double* coeffs, int degree) {
	std::cout << "[";
	for (int i = 0; i < degree; i++) {
		std::cout << coeffs[i] << ", ";
	}
	std::cout << coeffs[degree] << "]" << std::endl;
}

void PrintResults(double* coeffs, int degree, double rmse, double mape) {
	std::cout << std::fixed;
	
	std::cout << "CPU" << std::endl << "---" << std::endl;
	std::cout << "Degree: " << degree << std::endl;
	std::cout << "Coeffs: ";
	PrintCoeffs(coeffs, degree);
	std::cout << "RMSE: " << rmse << std::endl;
	std::cout << "MAPE: " << mape << std::endl;
	std::cout << std::endl;

	std::cout << std::scientific;
}

void WriteToFile(
	const std::string filename, 
	PredsWErrors cpu_preds_w_margins, PredsWErrors mem_preds_w_margins
) {
	std::ofstream outfile(filename);
	if (!outfile) {
		std::cerr << "Error: Cannot open file!" << std::endl;
		return;
	}

	outfile << std::setprecision(16);
	for (int i = 0; i < cpu_preds_w_margins.first.size(); i++) {
		outfile << cpu_preds_w_margins.first[i] << "," << cpu_preds_w_margins.second[i] << "," 
				<< mem_preds_w_margins.first[i] << "," << mem_preds_w_margins.second[i] << std::endl;
	}

	outfile.close();
}
