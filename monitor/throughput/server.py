import sys
import iperf3

from utils import get_ip_addr_current_node, read_ports_df

def main():
    # argv = ip address of remote node

    ip_remote_node = sys.argv[1]
    ports_df = read_ports_df()

    server = iperf3.Server()
    server.port = ports_df.at[get_ip_addr_current_node(), ip_remote_node]
    server.verbose = False

    while True:
        server.run()

if __name__ == "__main__":
    main()
