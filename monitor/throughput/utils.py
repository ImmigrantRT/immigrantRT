import os
import subprocess
import pandas as pd

absolute_path = lambda path : os.path.join(os.path.dirname(os.path.realpath(__file__)), path)

def get_ip_addr_current_node():
	result = subprocess.check_output(["hostname -I | awk '{print $1}'"], shell=True)
	ip = result.decode('utf-8').replace('\n', '')
	return ip

def read_ports_df():
	df = pd.read_pickle(absolute_path('../../data/ports.pkl'))
	return df
