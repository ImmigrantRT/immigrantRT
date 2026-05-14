#!/bin/bash
runtime=10000
deadline=100000
period=100000
rm task_list.txt
for i in $(docker container ls --format "{{.ID}}")
do 
	OUTPUT=$(docker inspect -f '{{.State.Pid}}' $i)
	if [ "${OUTPUT}" -gt 0 ]
	then
		chrt -d -T $runtime -D $deadline -P $period -p 0 "${OUTPUT}"
		echo "${OUTPUT}" >> task_list.txt
	fi
done
