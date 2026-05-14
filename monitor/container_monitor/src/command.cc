#include "command.hpp"

std::string ExecuteCommand(const std::string& cmd) {
	std::string result = ""; // to store the result of executing the command

	try {
		// open pipe to read the output of the command
		std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
		if (!pipe) {
			throw std::runtime_error("popen() failed!");
		}

		// read the output into the string
		char buffer[32];
		while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
			result.append(buffer);
		}

		// remove trailing \n
		if (!result.empty() && result[result.size() - 1] == '\n') {
			result.pop_back();
		}
	} catch (const std::exception& ex) {
		std::cerr << "Error executing command: " << ex.what() << std::endl;
	}

	return result;
}
