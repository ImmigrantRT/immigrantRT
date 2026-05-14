#!/bin/bash

script_dir=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")

t_timeout=600 # collect data for 10 minutes

sudo -v

# record resource usage
#while IFS=',' read -r id crit && [ -n "$id" ]
#do
#    sudo timeout $t_timeout python3 $script_dir/../../monitor/container_monitor.py $id &
while IFS= read -r info && [ -n "$info" ]
do
    IFS=',' read -r -a array <<< "$info"
    sudo timeout $t_timeout $script_dir/../../monitor/container_monitor/bin/exec $script_dir/../../data ${array[0]} $1 &
done < "$script_dir/../../data/containers.csv"

wait

ip=$(hostname -I | awk '{print $1}')

# compute and record max usage --> empty the contents of the stats file
while IFS=',' read -r id crit && [ -n "$id" ]
do
    folder="$script_dir/../../data/containers/$id"

    max_usage=$(python3 $script_dir/main.py $folder/stats.txt)
    echo "$ip,$max_usage" >> $folder/requirements.txt
    
    : > $folder/stats.txt
done < "$script_dir/../../data/containers.csv"
