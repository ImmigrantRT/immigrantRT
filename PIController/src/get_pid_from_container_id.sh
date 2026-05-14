#!/bin/bash

if [ "$#" -gt 1 ]
then
	p=$2
fi

ret=$1
dir=$(find /sys/fs/cgroup/machine.slice/ -name libpod-$ret*)
readarray -t StringArray <<<$(cat $dir/cgroup.procs)
for val in "${StringArray[@]}"
do
	echo $val | sudo tee -a $p > /dev/null
	ret+=$(echo ','$val)
done
echo $ret
