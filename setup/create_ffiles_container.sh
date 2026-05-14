#!/bin/bash

script_dir=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")
data_dir="$script_dir/../data"

id=$1
mkdir $data_dir/containers/$id
touch $data_dir/containers/$id/stats.txt # file permission: 664
touch $data_dir/containers/$id/stats_processed.csv
criticality=$((RANDOM % 4 + 1)) # 1 = critical, 2 = high, 3 = medium, 4 = low
echo "$id,$criticality" >> $data_dir/containers.csv
