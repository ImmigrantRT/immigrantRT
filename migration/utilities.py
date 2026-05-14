import subprocess
import os
import time

import config

absolute_path = lambda path : os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), path)

def run_cmd_aggregation(cmd):
    cmd_list = cmd.split()
    c = cmd_list[2] + ' ' + cmd_list[3]
    new_cmd_list = []
    new_cmd_list.append(cmd_list[0])
    new_cmd_list.append(cmd_list[1])
    new_cmd_list.append(c)
    new_cmd_list.append(cmd_list[4])
    print(new_cmd_list)
    result = subprocess.run(new_cmd_list, stdout=subprocess.PIPE, text=True)
    return result.stdout.replace("\t", "")

def run_cmd(cmd):
    if type(cmd) == str:
        cmd = cmd.split()
    result = subprocess.run(cmd, stdout=subprocess.PIPE, text=True)
    return result.stdout.replace("\t", "")

def run_cmd_no_wait(cmd):
    if type(cmd) == str:
        cmd = cmd.split()
    p = subprocess.Popen(cmd, shell=False, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    return p.pid

def write_pid(my_pid, pid):
    with open(absolute_path('data/framework_pid.txt'), 'w') as f:
        f.write(str(my_pid)+'\n'+str(pid))

def t_time():
    return time.time()

def cycle_counter():
    return int(t_time()-config.start_time)

def check_timeout():
    return cycle_counter()<config.timeout

def write_log(msg):
    with open(absolute_path("migration/migration.log"), 'a') as f:
        f.write(msg+'\n')

# get cpu and memory usage of container
def get_resource_usage(container_id):
    cpu = 0.0
    mem = 0.0
    with open(absolute_path('data/containers/'+container_id+'/stats.txt'), 'r') as f:
        for line in f:
            a = line.split()
            if len(a)==3:
                u = a[2].strip().split(',')
                if len(u)==2:
                    if float(u[0])>cpu:
                        cpu = float(u[0])
                    if float(u[1])>mem:
                        mem = float(u[1])
    return cpu,mem

def find_lowest_cpu_mem_cid(all_containers):
    c_name = ""
    cpu_mem = {}
    for cid in all_containers:
        cpu_mem[cid] = get_resource_usage(cid)
    for key, value in cpu_mem.items():
        if c_name=="":
            c_name = key
        else:
            if cpu_mem[c_name][0]<value[0] or (cpu_mem[c_name][0]==value[0] and cpu_mem[c_name][1]<value[1]):
                c_name = key
    return c_name,cpu_mem[c_name][0],cpu_mem[c_name][1]

def check_resource(container_id, cpu_usage, mem_usage):
    with open(absolute_path("PIController/log/report.csv")) as f:
        lines = f.read().splitlines()
    QoS=lines[-1].split(',')[3]
    if QoS.isdigit() and float(QoS) < 0.8:
        return False
    with open(absolute_path("data/self/stats.txt"), 'r') as f:
        lines = f.read().splitlines()
    for line in lines:
        a = line.split()
        u = a[2].split(',')
        if (u[0].isdigit() and float(u[0])+float(cpu_usage)>80) or (u[1].isdigit() and float(u[1])+float(mem_usage)>80):
            return False
    return True

def run_curve_fitting(ip):
    if ip=="":
        dir = absolute_path("data/other")
    else:
        dir = absolute_path("data/remote_nodes/"+ip)

    filename = dir + "/stats_processed.csv"
    with open(filename, 'r') as file:
        lines = file.readlines()
        last_n_lines = lines[-config.n_samples_curve_fitting:]

    filename = dir + "/stats_tail.csv"
    with open(filename, 'w') as file:
        for line in last_n_lines:
            file.write(line)

    cmd = absolute_path("prediction/curve_fitting/bin/exec") + " " + dir + " " + config.t_pred_curve_fitting
    #print("check_cf: " + cmd)
    run_cmd(cmd)

def do_checkpoint(pre_check, cid):
    if pre_check==1 and config.ARM==0:
        pid = run_cmd_no_wait("sudo podman container checkpoint -P -e " + \
                    absolute_path('data/checkpoints/pre_' + cid + '.tar.gz') + \
                    " " + cid)
        #run_cmd("sudo chown " + config.user + " " + \
        #        absolute_path('data/checkpoints/pre_' + cid + '.tar.gz'))
    if pre_check==0:
        if config.ARM==0:
            run_cmd("sudo podman container checkpoint --with-previous --tcp-established -e " + \
                    absolute_path('data/checkpoints/' + cid + '.tar.gz') + \
                    " " + cid)
        else:
            run_cmd("sudo podman container checkpoint --tcp-established -e " + \
                    absolute_path('data/checkpoints/' + cid + '.tar.gz') + \
                    " " + cid)
        run_cmd("sudo chown " + config.user + " " + \
                absolute_path('data/checkpoints/' + cid + '.tar.gz'))

def delete_cid_from_node(filepath, cid):
    with open(absolute_path(filepath), "r") as f:
        lines = f.readlines()
    with open(absolute_path(filepath), "w") as f:
        for line in lines:
            if not line.startswith(cid):
                f.write(line)

