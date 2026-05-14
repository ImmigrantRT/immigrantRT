#!/bin/bash

: << 'COMMENT'
	Args: 
	- IP address of the remote node
	- username on the remote node
COMMENT

script_dir=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")

source_file="$script_dir/../data/self/stats.txt"
dest_file="$script_dir/../data/remote_nodes/$1/stats.txt"

sudo -u $2 rsync -a $2@$1:$source_file $dest_file
