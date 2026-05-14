#!/bin/bash
cycle=0
cores=$(nproc)
if (( $cores > 1 )); then
	cores=1
fi
a="0 0"
while true
do
	if (( $cycle < $1 )); then
		ts=$(date +%s%N)
		while read -r line; do
			IFS=',' read -ra ll <<< "$line"
			echo [$(date '+%m/%d/%Y %H:%M:%S')] $(top -b -n 2 -d 0.9 -p ${ll[-1]} | tail -1 | awk "{print \$9/$cores\",\"\$10}") >> ${RTCdir}/data/containers/${ll[0]}/stats.txt &
		done < ${RTCdir}/PIController/task_list.txt
		if (( $cores == 1 )); then
			b=$(awk '/cpu0 /{print $2+$4,$2+$4+$5}' /proc/stat)
		else
			i=0
			b="0 0"
			while (( $i < $cores )); do
				bb=$(awk '/cpu'$i' /{print $2+$4,$2+$4+$5}' /proc/stat)
				b=$(echo $b $bb | awk '{print $1+$3,$2+$4}')
				i=$((i+1))
			done
		fi
		cpu=$(echo $a $b | awk '{split($0,aa); print 100*(aa[3]-aa[1])/(aa[4]-aa[2])}')
		mem=$(free | grep Mem | awk '{print $3/$2 * 100.0}')
		echo [$(date '+%m/%d/%Y %H:%M:%S')] $cpu,$mem >> ${RTCdir}/data/self/stats.txt
		a=$b
		s_t=$((1-($(date +%s%N) - $ts)/1000000000))
		if (( $s_t > 0 )); then sleep $s_t; fi
	else
		break
	fi
	sleep $((1-($(date +%s%N) - $ts)/1000000000))
	cycle=$((cycle+1))
done
