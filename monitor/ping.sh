#!/bin/bash

: << 'COMMENT'
	Args: 
	- IP address of the remote node
COMMENT

script_dir=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")

timestamp() {
  date +"[%m/%d/%Y %H:%M:%S.%N]" # ns precision
}

timeout 0.1 ping -c 1 -s 64 $1 > /dev/null 2>&1
status=$(( $? == 0 ? 1 : 0 ))
echo $(timestamp) $status >> $script_dir/../data/remote_nodes/$1/health.txt