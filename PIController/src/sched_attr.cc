#include "sched_attr.hpp"

int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags){
    return syscall(SYS_sched_setattr, pid, attr, flags);
}
int sched_getattr(pid_t pid, const struct sched_attr *attr, unsigned int size, unsigned int flags){
    return syscall(SYS_sched_getattr, pid, attr, size, flags);
}
