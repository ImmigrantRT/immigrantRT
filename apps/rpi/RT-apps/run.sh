#!/bin/bash
sudo podman rm $(sudo podman stop $(sudo podman ps -a -q  --filter ancestor=docker.io/kaobinchou/rtapps-image-rpi:latest --format="{{.ID}}"))
if [ "$#" -gt 0 ]
then
	n=$1
else
	n=1
fi

rm -rf $PWD/apps/rpi/RT-apps/run/
mkdir $PWD/apps/rpi/RT-apps/run/

#for i in {1..10}
for i in $( seq 1 $n )
do
	sudo podman --runtime runc run --privileged --cgroupns=private --tmpfs $PWD/apps/rpi/RT-apps/run/ -itd rtapps-image-rpi
done
