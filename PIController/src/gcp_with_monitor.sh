#!/bin/bash
#RTCdir=$PWD

if [ "$#" -gt 0 ]
then
	p=$1
	truncate -s0 $p
fi

file=$RTCdir
file+='/PIController/task_list.txt'
#while sleep 0.9
#do
	>$file
	chmod 766 $file
	for i in $(systemctl list-units | grep libpod)
	do
		id=${i%% *}
		if [[ $id == libpod-* && ! $id =~ libpod-conmon-* ]];
		then
			cid=$(echo ${id:7:12})
			echo $($RTCdir/PIController/src/get_pid_from_container_id.sh $cid $p) >> $file;
		fi
	done
#done
