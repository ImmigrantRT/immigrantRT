#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BILLION 1000000000L
#define MAX(a, b) (a > b ? a : b)

void run_commands(char* commands[], int n_commands) {
	for (int i = 0; i < n_commands; i++) {
		system(commands[i]);
	}
}

void repeat(double t, char* commands[], int n_commands) {
	while (1) {
		struct timespec res;
		clock_gettime(CLOCK_REALTIME, &res);
		long t1 = res.tv_nsec;

		run_commands(commands, n_commands);

		clock_gettime(CLOCK_REALTIME, &res);
		long t2 = res.tv_nsec;

		long t_sleep = MAX(t * BILLION - (t2 - t1), 0);

		struct timespec req, rem;
		req.tv_sec = t_sleep / BILLION;
		req.tv_nsec = t_sleep % BILLION;
		nanosleep(&req, &rem);
	}
}

int main(int argc, char* argv[]) {
	// argv = time, array of system commands

	double t = atof(argv[1]);

	char* commands[argc - 2];
	for (int i = 0; i < argc - 2; i++) {
		commands[i] = argv[i + 2];
	}

	repeat(t, commands, argc - 2);
	return 0;
}