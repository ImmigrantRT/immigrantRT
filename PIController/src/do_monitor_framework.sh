#!/bin/bash
cycle=0
#cores=$(nproc)
#cores=$((cores-15))
cores=1
n_containers=$2
if [ ! -f $PWD/data/framework_pid.txt ]; then
	echo "missing data/framework_pid.txt"
	exit 1
fi	
pids=$(cat $PWD/data/framework_pid.txt)
if [ -z "$pids" ]; then
	echo "missing pids"
	exit 1
fi
pidarray=($pids)
f_pid=${pidarray[0]}
p_pid=${pidarray[1]}
while true
do
	if (( $cycle < $1 )); then
		ts=$(date +%s%N)
		echo [$(date '+%m/%d/%Y %H:%M:%S')] $(top -b -n 2 -d 0.9 -p $f_pid | tail -1 | awk "{print \$9/$cores\",\"\$10}") >> $PWD/PIController/log/framework_stats_$n_containers.txt &
		echo [$(date '+%m/%d/%Y %H:%M:%S')] $(top -b -n 2 -d 0.9 -p $p_pid | tail -1 | awk "{print \$9/$cores\",\"\$10}") >> $PWD/PIController/log/controller_stats_$n_containers.txt &
	else
		break
	fi
	sleep $((1-($(date +%s%N) - $ts)/1000000000))
	cycle=$((cycle+1))
done
