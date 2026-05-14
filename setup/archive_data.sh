#!/bin/bash

script_dir=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")
data_dir="$script_dir/../data"

sudo -v

t=$(cat $data_dir/experiment_time.txt)
t=$(echo ${t%.*} | sed -e 's/[\[\/\:]//g' -e 's/ /_/g')

# compress and archive the data recorded during the previous run
tar --exclude={README.md,.gitignore} -czvf $script_dir/../archive/data_$t.tar.gz $data_dir

# clean up data/ folder for next run
folders=("self" "remote_nodes" "containers" "other" "checkpoints")
for folder in "${folders[@]}"; do
  rm -rf $data_dir/$folder/
done

files=("containers.csv" "experiment_time.txt")
# files+=("nodes.txt" "ports.pkl") # uncomment if using different nodes for the next experiment
for file in "${files[@]}"; do
  rm $data_dir/$file
done
