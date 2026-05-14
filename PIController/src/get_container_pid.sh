#!/bin/bash

file=$RTCdir
file+='/PIController/task_list.txt'
while sleep 1
do
	>$file
#	echo t
	for i in $(systemctl list-units | grep libpod)
	do
		id=${i%% *}
		if [[ $id == libpod-* && ! $id =~ libpod-conmon-* ]];
		then
			ids=$(echo ${id:7:12})
			readarray -t StringArray <<<$(cat /sys/fs/cgroup/machine.slice/$id/cgroup.procs)
			for val in "${StringArray[@]}"; do
				ids+=$(echo ','$val)
			done
			echo $ids >> $file;
		fi
	done
#	for i in $(sudo podman container ls --format "{{.ID}}")
#        do
#                #echo $i
#                pid=$(sudo podman inspect -f '{{.State.Pid}}' $i)
#                tid=$(pgrep -P $pid)
#                echo $i $pid $tid >> $file
#        done
done
