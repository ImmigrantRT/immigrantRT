import sys

with open(sys.argv[1], 'r') as f:
	lines = f.readlines()

cpu_usage = []
mem_usage = []

for line in lines:
	split = line.split(' ')[2].split(',')
	cpu_usage.append(float(split[0]))
	mem_usage.append(float(split[1]))

print(str(max(cpu_usage)) + ',' + str(max(mem_usage)))
