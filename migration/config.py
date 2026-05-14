import platform
import os
import socket

architecture = platform.machine()
ARM=0
if architecture.startswith('arm') or architecture.startswith('aarch'):
    ARM=1
elif architecture.startswith('x86') or architecture.startswith('AMD'):
    ARM=0
else:
    #print("Unknown architecture")
    ARM=0

meminfo = dict((i.split()[0].rstrip(':'),int(i.split()[1])) for i in open('/proc/meminfo').readlines())
total_mem = meminfo['MemTotal']

current_ip = socket.gethostbyname(socket.gethostname())

timeout=0
do_migration=0
do_control=0
start_time=0
n_samples_curve_fitting=0
t_pred_curve_fitting=""
remote_nodes=[]
user=""
port=12345
cpuaf_tasks=""
