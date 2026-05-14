#include <sys/resource.h>
#include <unistd.h>
#include <iostream>
int main(){
    int which = PRIO_PROCESS;
    id_t pid = 1541586;
    int ret;


    //pid = getpid();
    ret = getpriority(which, pid);
    std::cout <<  pid << ": " << ret << std::endl;

    ret = setpriority(which, pid, 10);
    std::cout << ret << std::endl;
    ret = getpriority(which, pid);
    std::cout <<  pid << ": " << ret << std::endl;

    return 0;
}
