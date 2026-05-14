#!/bin/bash

script_dir=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")

# generate rsa id pair at ~/.ssh using no passphrase
# if .ssh/id_rsa pair is present, no changes are made
cat /dev/zero | ssh-keygen -q -N ""

# authorize the current node by installing .ssh/id_rsa.pub as an authorized key on other known nodes
while IFS= read -r ip && [ -n "$ip" ]
do
	ssh-copy-id -i ~/.ssh/id_rsa.pub -o StrictHostKeyChecking=no $USER@$ip
done < "$script_dir/../data/nodes.txt"
