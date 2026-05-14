#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>
#include <iostream>
#include <linux/sched.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <stdint.h>

struct sched_attr {
    uint32_t size;
    uint32_t sched_policy;
    uint64_t sched_flags;
    int32_t sched_nice;
    uint32_t sched_priority;
    uint64_t sched_runtime;
    uint64_t sched_deadline;
    uint64_t sched_period;
};
int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags){
    return syscall(SYS_sched_setattr, pid, attr, flags);
}
int sched_getattr(pid_t pid, const struct sched_attr *attr, unsigned int size, unsigned int flags){
    return syscall(SYS_sched_getattr, pid, attr, size, flags);
}

void * threadA(void *p) {
    pthread_t thId = pthread_self();
    std::cout << "Thread Id from thread function : " << thId << std::endl;

    int ret;
    struct sched_attr attr;

    ret = sched_getattr(0, &attr, sizeof(attr), 0);
    if (ret < 0){
            perror("getattr1 failed.");
            //return;
    }

    attr.sched_policy = SCHED_DEADLINE;
    //attr.sched_flags = SCHED_FLAG_DL_OVERRUN;
    attr.sched_runtime = 10*1000;
    attr.sched_period = 2.1*1000*1000*1000;
    attr.sched_deadline = 11*1000;

    ret = sched_setattr(0, &attr, 0);
    if (ret < 0){
            perror("setattr failed.");
            //return;
    }

    for(;;){
	float f1 = 1.37997234238/3.8324134213457827729;
	float f2 = 1.37997234238/3.8324134213457827729;
	float f3 = 1.37997234238/3.8324134213457827729;
	float f4 = 1.37997234238/3.8324134213457827729;
        printf("sensor\n");
        fflush(0);
        sched_yield();
    }
}

int main(){
    pid_t pid = syscall(__NR_gettid);;
    int ret;

    struct sched_attr attr;

    pthread_t pthreadA;
    pthread_create(&pthreadA, NULL, threadA, NULL);
    std::cout << pid << std::endl;

    while(1){
        ret = sched_getattr(pid, &attr, sizeof(attr), 0);
        if (ret < 0){
	    perror("getattr2 failed.");
            return(-1);
        }
        std::cout << attr.sched_flags << std::endl;
        usleep(2000*1000);
    }

    //ret = setpriority(which, pid, 10);
    //std::cout << ret << std::endl;
    //ret = getpriority(which, pid);
    //std::cout <<  pid << ": " << ret << std::endl;

    return 0;
}
