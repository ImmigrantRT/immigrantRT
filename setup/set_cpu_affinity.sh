#!/bin/bash

# $1 should be the number of cores to be assigned
if [ $1 = "1" ]
then
	cores="0"
else
	cores="0-$(($1 - 1))"
fi

# Use deflaut cgroup folder (/sys/fs/cgroup) or the folder from $2
if [ "$#" -gt 0 ]
then
	p=$(echo ${2%/})
else
	p=/sys/fs/cgroup
fi

if [ ! -d "$p/cpuset" ]
then
	mkdir $p/cpuset
fi
mount -t cgroup -o cpuset cpuset $p/cpuset
# Make sub-folder for Real-Time tasks
if [ ! -d "$p/cpuset/realtime" ]
then
	mkdir -p $p/cpuset/realtime
fi

# Assign cpus for Real-Time tasks (Here we use cpu0 and cpu1)
echo $cores | sudo tee $p/cpuset/realtime/cpuset.cpus > /dev/null

# Assign memory nodes for Real-Time tasks (Here we use node0 and node1)
echo 0-1 | sudo tee $p/cpuset/realtime/cpuset.mems > /dev/null

# Set cpu exclusive for other cpus
echo 1 | sudo tee $p/cpuset/cpuset.cpu_exclusive > /dev/null

# Disable load balance for scheduler
echo 0 | sudo tee $p/cpuset/cpuset.sched_load_balance > /dev/null

# Set cpu exclusive for Real-Time tasks
echo 1 | sudo tee $p/cpuset/realtime/cpuset.cpu_exclusive > /dev/null

# Set memory exclusive for Real-Time tasks
echo 1 | sudo tee $p/cpuset/realtime/cpuset.mem_exclusive > /dev/null

# Assign pids to this cpu set (Here we assign the pid of current shell)
#echo $BASHPID | sudo tee $p/cpuset/realtime/tasks > /dev/null
