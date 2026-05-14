#!/bin/bash
if [ ! -d log ]; then
	mkdir log
fi

for n in {2..1000}
do
	#n=700
	rm log/$n.csv
	> log/$n.csv
	for ((i=0; i<$n; i++))
	do
		python3 threads.py $n $i &
	done
	while pgrep -x python3 > /dev/null; do
		sleep 1
	done
	echo end of $n simulation.
done
