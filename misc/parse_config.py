import sys
import os
import yaml

def read_config():
	with open(os.path.join(os.path.dirname(os.path.realpath(__file__)), '../launch/config.yaml'), 'r') as f:
		data = f.read().replace('\t', '  ')
		data = yaml.load(data, yaml.SafeLoader) 

	return data

def get_param(data, path):
	try:
		val = data
		for key in path.split('.'):
			val = val[key]
		
		return val
	
	except KeyError:
		print('Unable to get ' + path)
		return None

if __name__ == "__main__":
	path = sys.argv[1]	# '.'-separated string
	val = get_param(read_config(), path)
	print(val)
