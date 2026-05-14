import os
import subprocess
import ipaddress
import pandas as pd
import itertools

absolute_path = lambda path : os.path.join(os.path.dirname(os.path.realpath(__file__)), path)

def get_ip_addr_current_node():
	result = subprocess.check_output(["hostname -I | awk '{print $1}'"], shell=True)
	ip = result.decode('utf-8').replace('\n', '')
	return ip

def read_ip_addr_remote_nodes():
	with open(absolute_path('../data/nodes.txt'), 'r') as f:
		ip = f.read().splitlines()
	return ip

def assign_ports(ip_addresses):
	ip_addresses = sorted(ip_addresses, key=ipaddress.IPv4Address)
		
	df = pd.DataFrame(0, index=ip_addresses, columns=ip_addresses)
	port = 23456 # starting port number (unreserved)

	for ip1, ip2 in itertools.permutations(ip_addresses, 2):
		df.at[ip1, ip2] = port
		port += 1

	return df

def main():
	ip_host = get_ip_addr_current_node()
	ip_remote_nodes = read_ip_addr_remote_nodes()

	df = assign_ports([ip_host] + ip_remote_nodes)
	df.to_pickle(absolute_path('../data/ports.pkl'))
	
if __name__ == "__main__":
	main()
