#!/bin/bash

script_dir=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")
data_dir="$script_dir/../data"

# read value of timeout from config.yaml
config () { echo "$(python3 $script_dir/parse_config.py $1)" ; }
t_timeout=$(config 'timeout')

if ! grep -q "$1" $data_dir/containers.csv; then
    echo $1 >> $data_dir/containers.csv

    # create folder and necessary files
    mkdir $data_dir/containers/$1
    touch $data_dir/containers/$1/stats.txt
    touch $data_dir/containers/$1/stats_processed.csv

    # launch container monitor
    sudo timeout $t_timeout python3 $script_dir/../monitor/container_monitor.py $1 &
fi
