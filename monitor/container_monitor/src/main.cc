#include <iostream>
#include <chrono>
#include <thread>

#include "command.hpp"
#include "stats.hpp"
#include "utilities.hpp"

std::string GetTimestamp() {
	std::string format = "[%m/%d/%Y %H:%M:%S.%N]";
	std::string cmd = "date '+" + format + "'";

	return ExecuteCommand(cmd);
}

int main(int argc, char *argv[]) {
	// argv = data folder, container ID

	if (argc != 4) {
		std::cout << "3 arguments needed" << std::endl;
		return 1;
	}

	std::string data_folder = argv[1];
	std::string container_id = argv[2];
	int cpus = std::atoi(argv[3]);

	while (true) {
		std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

		std::string stats = GetPodmanStats(container_id, cpus);
		// std::cout << "Stats: " << stats << std::endl;

		std::string ts = GetTimestamp();
		// std::cout << "Timestamp: " << ts << std::endl;

		std::string filename = data_folder + "/containers/" + container_id + "/stats.txt";
		WriteToFile(filename, ts, stats);

		std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
		std::chrono::duration<double> t_elapsed = t2 - t1;

		double t_rem = 1 - t_elapsed.count();
		if (t_rem > 0) {
			std::this_thread::sleep_for(std::chrono::duration<double>(t_rem));
		} else {
			// no sleep needed, command execution took more than 1s
			continue;
		}

	}

	return 0;
}
