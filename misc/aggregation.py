import sys
import os
import numpy as np
from datetime import datetime

data_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), '../data')

def get_datetime_obj(timestamp):
	timestamp = timestamp[:26] # keep first 6 digits after the decimal
	return datetime.strptime(timestamp, '%m/%d/%Y %H:%M:%S.%f')

def read_container_ids():
	with open(os.path.join(data_path, 'containers.csv'), 'r') as file:
		lines = file.readlines()

	return [line.split(',')[0] for line in lines]

def read_stats(file_path, ref_time, return_time : bool):
	last_line = ''

	if not os.path.exists(file_path) or os.path.getsize(file_path) == 0: # check if file does not exist or file is empty
		if return_time:
			return 0,np.array([0.0,0.0])
		else:
			return np.array([0.0,0.0])

	with open(file_path, 'rb') as file: # open in binary mode
		file.seek(-2, 2) # second last byte
		while file.read(1) != b'\n': # read until \n
			file.seek(-2, 1) # move one byte backward

		last_line = file.readline().decode()

	if return_time:
		split = last_line.split(']')

		time_elapsed = get_datetime_obj(split[0][1:]) - ref_time
		time_elapsed = round(time_elapsed.total_seconds())

		cpu_str, mem_str = split[1].strip().split(',')

		return time_elapsed, np.array([float(cpu_str), float(mem_str)])

	else:
		cpu_mem_str = last_line.split(' ')[2]
		cpu_str, mem_str = cpu_mem_str.split(',')

		return np.array([float(cpu_str), float(mem_str)])

def main(ref_time, total_mem):
	container_ids = read_container_ids()

	# availability of the current node
	time_elapsed, node_stats = read_stats(os.path.join(data_path, 'self/stats.txt'), ref_time, True)

	usage = np.array([100, total_mem]) - node_stats

	# subtract usage of the containers
	for id in container_ids:
		usage -= read_stats(os.path.join(data_path, 'containers/' + id + '/stats.txt'), ref_time, False)

	with open(os.path.join(data_path, 'other/stats_processed.csv'), 'a') as f:
		f.write(f"{time_elapsed},{usage[0]},{usage[1]}\n")

if __name__ == '__main__':
	ref_time = get_datetime_obj(sys.argv[1].strip('[]'))
	total_mem = float(sys.argv[2])

	main(ref_time, total_mem)
