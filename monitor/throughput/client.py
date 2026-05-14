import sys
import os
import iperf3
import pandas as pd

from utils import get_ip_addr_current_node, read_ports_df

absolute_path = lambda path : os.path.join(os.path.dirname(os.path.realpath(__file__)), path)

def main():
	# argv = ip address of remote node

	ip_remote_node = sys.argv[1]
	ports_df = read_ports_df()

	client = iperf3.Client()
	client.duration = 1
	client.server_hostname = ip_remote_node
	client.port = ports_df.at[ip_remote_node, get_ip_addr_current_node()]
	client.num_streams = 1
	client.zerocopy = True
	result = client.run()

	file = absolute_path('../../data/remote_nodes/' + ip_remote_node + '/throughput.txt')
	with open(file, 'w') as f:
		f.write(str(result.sent_MB_s))

if __name__ == "__main__":
	main()
