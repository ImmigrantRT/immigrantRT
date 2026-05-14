# first of all import the socket library
import socket
from socket import AF_INET, SOCK_DGRAM
import os
import yaml
import threading
import sys
import queue

import setup.assign_ports
import setup.set_container_requirement
sys.path.append('migration/')
import utilities
from threads import client_handler, server_handler
import config

# Check parameters
#if len(sys.argv)<3:
#    print("Usage: python3 run.py [do_control?1:0] [do_migration?1:0]")
#    sys.exit()
#do_control=sys.argv[1]
#do_migration = do_control if do_control=='0' else sys.argv[2]
#config.do_control=do_control
#config.do_migration=do_migration

script_dir = os.path.dirname(os.path.abspath('__file__'))
#print(script_dir)
os.environ["RTCdir"] = script_dir

with open(utilities.absolute_path('launch/config.yaml'), 'r') as f:
    data = f.read().replace('\t', '  ')
    conf = yaml.load(data, yaml.SafeLoader)
    config.user = conf['user']
    config.timeout = float(conf['timeout'])
    config.do_control = int(conf['do_control'])
    config.do_migration = int(conf['do_migration'])
    do_requirement = int(conf['do_requirement'])
    cpus = conf['cpuaffinity']
    cpuaffinity = conf['cpuaffinity_folder']
    if cpuaffinity == "":
        cpuaffinity = "/sys/fs/cgroup/"
    config.cpuaf_tasks = os.path.join(cpuaffinity,"cpuset/realtime/tasks")
    config.n_samples_curve_fitting = int(conf['prediction']['curve_fitting']['samples'])
    config.t_pred_curve_fitting = str(conf['prediction']['curve_fitting']['prediction_time'])

# next create a socket object
#s = socket.socket(AF_INET,SOCK_DGRAM)
s = socket.socket()
print ("Socket successfully created")

# reserve a port on your computer in our
# case it is 12345 but it can be anything
#port = 12345

# Next bind to the port
# we have not typed any ip in the ip field
# instead we have inputted an empty string
# this makes the server listen to requests
# coming from other computers on the network
s.bind(('', config.port))
print ("socket binded to %s" %(config.port))

# put the socket into listening mode
s.listen(5)
print ("socket is listening")

# Archive the contents and clean the data/ folder
print ("archiving prervious contents...")
utilities.run_cmd("sudo " + utilities.absolute_path("setup/archive_data.sh"))

# Create folders and files needed for running the framework
print ("creating folders and files of each container...")
utilities.run_cmd(utilities.absolute_path("setup/create_folders_and_files.sh"))

# Assign ports for throughput measurement
setup.assign_ports.main()

# Start the iperf3 server
print ("starting iperf3 server...")
utilities.run_cmd_no_wait("sudo " + utilities.absolute_path("launch/run_servers.sh"))

# Set CPU affinity
#utilities.run_cmd("sudo " + utilities.absolute_path("setup/set_cpu_affinity.sh") + " " + str(cpus) + " " + cpuaffinity)

# Set CPU affinity to containers
cmd = "sudo -E " + utilities.absolute_path("PIController/src/gcp_with_monitor.sh") + " " + config.cpuaf_tasks
utilities.run_cmd(cmd)

# Estimate the conatiner resource requirement
if do_requirement == 1:
    print ("estimating container resource requirement...")
    utilities.run_cmd(utilities.absolute_path("experiments/container_requirements/run.sh") + " " + str(cpus))

# Send the requirements from current node to remote nodes
if do_requirement == 1:
    print ("sending requirements to remote nodes...")
    setup.set_container_requirement.send_requirement()

# Clear log files
open(utilities.absolute_path("data/migration.log"), 'w').close()
open(utilities.absolute_path("migration/migration.log"), 'w').close()
#open(utilities.absolute_path("PIController/log/error_msg.txt"), 'w').close()

# Clear dmesg
utilities.run_cmd("sudo dmesg -c")

# Gather container IDs
all_containers = []
with open(utilities.absolute_path("data/containers.csv"), 'r') as f:
    lines = f.read().splitlines()
for line in lines:
    ss = line.split(',')
    if len(ss)==2:
        all_containers.append(ss[0])

config.start_time = utilities.t_time()

# Send ready to launch signal to remote nodes
print ("sending ready message to remote nodes...")
with open(utilities.absolute_path("data/nodes.txt"), 'r') as f:
    all_nodes=f.read().splitlines()
config.remote_nodes = all_nodes

# Create the shared queue and launch both threads 
q = {}
for n in all_nodes:
    q[n] = queue.Queue()

# creating a lock
lock = threading.Lock()

rs = {} # ready signal received or not from each node
for n in all_nodes:
    print ("sending ready to "+n)
    threading.Thread(target=client_handler, args=(n,lock,q[n],)).start()
    rs[n] = 0
print (rs)

config.start_time = utilities.t_time()
tt = utilities.cycle_counter()

# a loop until reach timeout or an error occurs
print ("waiting remote nodes ready...")
while tt < config.timeout:

    print("time=",utilities.cycle_counter()," in main()")
    # Establish connection with client.
    s.settimeout(int(config.timeout-tt)+1)
    c = 0
    addr=0
    try:
        c, addr = s.accept()
        print ('Got connection from', addr )
        threading.Thread(target=server_handler, args=(q,c,addr,rs,)).start()
    except socket.timeout:
        sys.exit()

    tt = utilities.cycle_counter()

