#!/bin/bash
sudo podman rm $(sudo podman stop $(sudo podman ps -a -q  --filter ancestor=docker.io/kaobinchou/rtapps-image:latest --format="{{.ID}}"))
#if [ "$#" -gt 0 ]
#then
#	n=$1
#else
#	n=1
#fi

rm -rf $PWD/apps/RT-apps/run/
mkdir $PWD/apps/RT-apps/run/

file=$PWD/PIController/task_list.txt
>$file

cid_1=""

for i in {1..100}
#for i in $( seq 1 $n )
do
	cid=$(sudo podman --runtime runc run --privileged --cgroupns=private --tmpfs $PWD/apps/RT-apps/run/ -itd rtapps-image)
	cid="${cid:0:12}"
	if [ -z "$cid_1" ]; then
		cid_1=$cid
		sleep 630
	else
		$PWD/setup/create_ffiles_container.sh $cid
		cp $PWD/data/containers/$cid_1/requirements.txt $PWD/data/containers/$cid/
		cpuaf=/mydata/cpuset/realtime/tasks
		echo $(sudo $PWD/PIController/src/get_pid_from_container_id.sh $cid $cpuaf) >> $file;
	fi
	$PWD/PIController/src/do_monitor_framework.sh 60 $i
	#sleep 60
done
