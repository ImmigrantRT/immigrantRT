#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#include "sched_attr.hpp"

std::string RTCdir(std::getenv("RTCdir"));

int main(int argc, char** argv){
    int ret;
    struct sched_attr attr;
    std::string cmd = RTCdir+"/PIController/src/get_container_pid.sh";

    if (argc < 2) {
        std::cerr << "Usage: ./create_task_list.txt cycles_in_second" << std::endl;
	return -1;
    }

    int cycles = atoi(argv[1]);
    int cycle_count=0;

    ret = sched_getattr(0, &attr, sizeof(attr), 0);
    if (ret < 0){
        perror("create task_list.txt: getattr failed.");
	return -1;
    }

    attr.sched_policy = SCHED_DEADLINE;
    attr.sched_runtime = 1000*1000;
    attr.sched_period = 1000*1000*1000;
    attr.sched_deadline = attr.sched_period;

    ret = sched_setattr(0, &attr, 0);
    if (ret < 0){
        perror("create task_list.txt: setattr failed.");
	return -1;
    }

    while (cycles==0 || cycle_count<cycles) {
        //std::cout << "create task_list.txt: check " << cycle_count << std::endl;
        int ret = system(cmd.c_str());
	if (ret<0) std::cerr << "create task_list.txt error: " << ret << std::endl;
	cycle_count++;
        sched_yield();
    }

    return 0;
}
