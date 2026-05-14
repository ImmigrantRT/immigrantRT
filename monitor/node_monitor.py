import os
import subprocess
import re
import time
import sys

absolute_path = lambda path : os.path.join(os.path.dirname(os.path.realpath(__file__)), path)

def read_file(filename):
	with open(filename, 'r') as f:
		data = f.read()
	return data

def get_timestamp():
	result = subprocess.run(['date', '+[%m/%d/%Y %H:%M:%S.%N]'], stdout=subprocess.PIPE)
	ts_bytes = result.stdout
	return ts_bytes.decode("utf-8")[:-1]

def string_to_int_list(data):
    split = data.split(' ')
    ret = []
    for s in split:
        try:
            ret.append(int(s))
        except ValueError:
            pass
    return ret

def get_cpu_cycles(data, cpus):
	#data = data[:data.index('\n')]
	#data = data[5:] # remove first word
    data_list = data.split('\n')
    idle_cycles=0
    total_cycles=0
    for i in range(int(cpus)):
        data = data_list[i+1]
        split = string_to_int_list(data)
        idle_cycles = idle_cycles + split[3]
        total_cycles = total_cycles + sum(split)

	#split = data.split(' ')
	#idle_cycles = int(split[3])

	#data = data.replace(' ', '+')
	#total_cycles = eval(data)

    return (total_cycles, idle_cycles)

def compute_cpu_usage(total_cycles_curr, idle_cycles_curr, total_cycles_prev, idle_cycles_prev):
	total_cycles_delta = total_cycles_curr - total_cycles_prev
	idle_cycles_delta = idle_cycles_curr - idle_cycles_prev

	usage = (total_cycles_delta - idle_cycles_delta) / total_cycles_delta * 100
	return usage

def get_available_memory(data):
	split1 = data.split('\n')
	mem_available_info = split1[2]
	split2 = re.split(' +', mem_available_info)
	return split2[1]

def main():
	cpus = sys.argv[1]
	cpu_data_prev = read_file('/proc/stat')
	(total_cycles_prev, idle_cycles_prev) = get_cpu_cycles(cpu_data_prev, cpus)
	time.sleep(1)

	while True:
		t1 = time.time()

		# % CPU available
		cpu_data_curr = read_file('/proc/stat')
		(total_cycles_curr, idle_cycles_curr) = get_cpu_cycles(cpu_data_curr, cpus)
		cpu_usage = compute_cpu_usage(total_cycles_curr, idle_cycles_curr, total_cycles_prev, idle_cycles_prev)
		(total_cycles_prev, idle_cycles_prev) = (total_cycles_curr, idle_cycles_curr)

		# memory available (in KB)
		mem_data = read_file('/proc/meminfo')
		mem_available = get_available_memory(mem_data)

		ts = get_timestamp()
		stats = ts + ' ' + str(100 - cpu_usage) + ',' + str(mem_available) + '\n'

		with open(absolute_path('../data/self/stats.txt'), 'a') as f:
			f.write(stats)

		t2 = time.time()
		time.sleep(max(1 - (t2 - t1), 0))

if __name__ == "__main__":
	main()
