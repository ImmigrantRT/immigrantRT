#!/bin/bash

trap "kill 0" EXIT

script_dir=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")

# start iperf3 servers
while IFS= read -r ip && [ -n "$ip" ]
do
	python3 $script_dir/../monitor/throughput/server.py $ip &
done < "$script_dir/../data/nodes.txt"

# start migration server
#python3 migration/server.py &

wait
