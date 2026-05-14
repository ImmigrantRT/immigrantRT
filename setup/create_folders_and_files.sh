#!/bin/bash

script_dir=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")
data_dir="$script_dir/../data"

# for current node
mkdir $data_dir/self
touch $data_dir/self/stats_processed.csv
touch $data_dir/self/QoS.csv # file permission: 664

# for remote nodes
mkdir $data_dir/remote_nodes

while IFS= read -r ip && [ -n "$ip" ]
do
	mkdir $data_dir/remote_nodes/$ip
	touch $data_dir/remote_nodes/$ip/stats_processed.csv
	touch $data_dir/remote_nodes/$ip/health.txt
	touch $data_dir/remote_nodes/$ip/lines.txt
	touch $data_dir/remote_nodes/$ip/throughput.txt
done < "$data_dir/nodes.txt"

# for podman containers
touch $data_dir/containers.csv
mkdir $data_dir/containers

sudo podman ps | awk 'NR>=2{print $1}' | while read id
do
	$script_dir/create_ffiles_container.sh $id
	#criticality=$((RANDOM % 4 + 1)) # 1 = critical, 2 = high, 3 = medium, 4 = low
	#echo "$id,$criticality" >> $data_dir/containers.csv
	#mkdir $data_dir/containers/$id
	#touch $data_dir/containers/$id/stats.txt # file permission: 664
	#touch $data_dir/containers/$id/stats_processed.csv
done

# for checkpoints
mkdir $data_dir/checkpoints

# for keeping track of the usage of other apps
mkdir $data_dir/other
