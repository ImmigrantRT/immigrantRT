import socket
from socket import AF_INET, SOCK_DGRAM
import os
import threading
import queue
import time

import utilities
import config 

def client_handler(node, lock, q):
    global res
    res = 0
    s = socket.socket()
    # connect to node and send ready signal
    while True:
        try:
            s.connect((node, config.port))
            s.send("ready".encode())
            if s.recv(1024).decode()=='3':
                break
        except socket.error:
            time.sleep(1)

    # send mgration request
    while utilities.check_timeout():
        data = ""
        status = 0
        try:
            data = q.get(timeout=1)
        except queue.Empty:
            continue
        msg = "["+str(utilities.cycle_counter())+"] migration request: "+data+" in client_handler"
        utilities.write_log(msg)
        #cid = data.split(',')[0]
        print("check data="+data)
        cid = data
        if config.ARM==0:
            while not os.path.exists(utilities.absolute_path('data/checkpoints/pre_' + cid + '.tar.gz')):
                time.sleep(0.5)
            utilities.run_cmd("sudo chown " + config.user + " " + \
                    utilities.absolute_path('data/checkpoints/pre_' + cid + '.tar.gz'))
            print("send 0")
            s.send(("0,"+data).encode())
            if s.recv(1024).decode()=='0':
                status = 1
            print("recv 0")
        else:
            status = 1
        if status == 1:
            print("try to get lock")
            if lock.acquire(timeout=10):
                print("get lock")
                utilities.write_log("lock acquired: "+node+" in client_handler")
                if res==0 and utilities.check_timeout():
                    cycle = utilities.cycle_counter()
                    msg = str(cycle)+": do checkpoint"
                    utilities.write_log(msg)
                    utilities.do_checkpoint(0, cid)
                    s.send(("1,"+data).encode())
                    if s.recv(1024).decode()=='1':
                        s.send(("2,"+cid).encode())
                        if s.recv(1024).decode()=='2':
                            msg = str(utilities.cycle_counter())+": finalized migration "+cid
                            utilities.write_log(msg)
                            utilities.run_cmd("sudo podman rm "+cid)
                            res = 1
                            utilities.delete_cid_from_node("data/containers.csv", cid)
                            utilities.delete_cid_from_node("PIController/task_list.txt", cid)
                            #cycle = cycle_counter()
                            with open(utilities.absolute_path("data/migration.log"), 'a') as the_file:
                                the_file.write('1,'+str(cycle)+','+cid+','+node+'\n')
                            #open(absolute_path("data/self/QoS.csv"), 'w').close()
                            msg = "miigration spend time: "+str(utilities.cycle_counter()-cycle)
                            utilities.write_log(msg)
                else:
                    res = 0
                lock.release()
            else:
                utilities.write_log("lock timeout: "+node+" in client_handler")
            s.send(("3,"+cid).encode())

def migration_check_handler(q):
    time.sleep(30)
    mig_cooldown = 0
    #print("check migration_check_handler")
    while utilities.check_timeout():
        c_start_time = utilities.t_time()
        if mig_cooldown==0:
            cid = ""
            file = open(utilities.absolute_path("data/experiment_time.txt"), "r")
            t_reference = file.readline()
            file.close()
            cmd = ["python3", utilities.absolute_path("misc/aggregation.py"), t_reference.strip(), str(config.total_mem)]
            #print(cmd)
            utilities.run_cmd(cmd)
            utilities.run_curve_fitting("")
            cmd = utilities.absolute_path("analysis/trigger/bin/exec") + " " + utilities.absolute_path("data") + " " + config.current_ip
            #print("check 0: " + cmd)
            cid = utilities.run_cmd(cmd).strip()
            print("check 1: " + cid + " " + str(len(cid)))
            if len(cid) == 12:
                #cmd = "bash " + utilities.absolute_path("launch/run.sh") + " validate_candidates " + cid
                with open(utilities.absolute_path("data/containers/"+cid+"/rank.txt")) as f:
                    lines = f.readlines()
                read_preds = 0
                for ip in lines:
                    ip = ip.strip()
                    dir = utilities.absolute_path("data/remote_nodes/"+ip)
                    with open(dir+"/stats.txt") as f:
                        stats = f.readlines()
                    num_lines_stats = len(stats)
                    with open(dir+"/lines.txt") as f:
                        num_lines_processed = f.readline()
                    if num_lines_processed=="":
                        num_lines_processed=0
                    if num_lines_stats - int(num_lines_processed) > 0:
                        cmd = [utilities.absolute_path("misc/processing/bin/exec"), dir, t_reference]
                        #print(cmd)
                        utilities.run_cmd(cmd)
                        #print("check 2")
                        utilities.run_curve_fitting(ip)
                        #print("check 3")
                        read_preds = 1
                cmd = utilities.absolute_path("analysis/validation/bin/exec") + " " + utilities.absolute_path("data") + \
                        " " + cid + " " + str(read_preds)
                #print("check 4: " + cmd)
                utilities.run_cmd(cmd)
            #with open(utilities.absolute_path("data/self/QoS.csv"), 'r') as f:
            #    lines = f.read().splitlines()
            #ss = lines[-1].split(',')
            #if len(ss)==3:
            #    cid = ss[1]
                if len(cid)==12:
                    #if config.ARM==0:
                    #    pid = utilities.run_cmd_no_wait("sudo podman container checkpoint -P -e " + \
                    #            utilities.absolute_path('data/checkpoints/pre_' + cid + '.tar.gz') + \
                    #            " " + cid)
                    #cpu_cid,mem_cid = utilities.get_resource_usage(cid)
                    #msg = cid+","+str(cpu_cid)+","+str(mem_cid)
                    utilities.do_checkpoint(1, cid)
                    msg = cid
                    with open(utilities.absolute_path("data/containers/"+cid+"/rank.txt")) as f:
                        lines = f.readlines()
                    #for key in q:
                    for key in lines:
                        print("key="+key.strip()+",msg="+msg)
                        q[key.strip()].put(msg)
                    mig_cooldown=10
                    continue
            #with open(utilities.absolute_path("data/self/stats.txt"), 'r') as f:
            #    lines = f.read().splitlines()
            #ss = lines[-1].split()
            #if len(ss)==3:
            #    ss = ss[2].split(',')
            #    if len(ss)==2:
            #        cpu = ss[0]
            #        mem = ss[1]
            #        if float(cpu)>80 or float(mem)>80:
            #            cid,cpu_cid,mem_cid = utilities.find_lowest_cpu_mem_cid()
            #            pid = utilities.run_cmd_no_wait("sudo podman container checkpoint -P -e " +
            #                    utilities.absolute_path('data/checkpoints/pre_' + cid + '.tar.gz') +
            #                    " " + cid)
            #            msg = cid+","+str(cpu_cid)+","+str(mem_cid)
            #            for key in q:
            #                q[key].put(msg)
            #            mig_cooldown=10
            #            continue

        if mig_cooldown>0:
            mig_cooldown=mig_cooldown-1

        c_time = utilities.t_time()-c_start_time
        if c_time<1:
            time.sleep(1-c_time)

def server_handler(q, c, addr, rs):
    while utilities.check_timeout():
        #msg = "time="+str(cycle_counter())+" in server_handler"
        #write_log(msg)
        #print(msg)
        rec_data = c.recv(1024).decode()
        cmd = rec_data.split(',')
        if cmd[0]=="ready":
            rs[addr[0]] = 1
            print (addr[0] + " is ready!")
            c.send('3'.encode())
            cs = 1 # check if received ready from all nodes
            for n in config.remote_nodes:
                if rs[n] == 0:
                    cs = 0
            #print (rs)
            if cs:
                print ("all nodes ready, starting framework now...")
                if os.path.isfile(utilities.absolute_path("PIController/log/error_msg.txt")):
                    os.remove(utilities.absolute_path("PIController/log/error_msg.txt"))
                utilities.run_cmd("sudo dmesg -c")
                #cmd = "sudo -E " + absolute_path("PIController/bin/PIController") + " " + str(int(config.timeout)) + " " + str(config.do_control)
                cmd = utilities.absolute_path("launch/run.sh") + " " + str(config.do_control)
                pid = utilities.run_cmd_no_wait(cmd)
                my_pid = os.getpid()
                utilities.write_pid(my_pid, pid)
                #print("1 start_time ",start_time)
                config.start_time = utilities.t_time()
                #print("2 start_time ",start_time)
                print(config.do_migration)
                if str(config.do_migration)=='1':
                    #print("start migration_check_handler")
                    threading.Thread(target=migration_check_handler, args=(q,)).start()
        elif cmd[0]=='0': # Copy pre-checkpoint data
            if config.ARM==0:
                print(utilities.run_cmd("sudo -u " + config.user + " rsync -aqz " + config.user + "@" + addr[0] + ":" + \
                        utilities.absolute_path('data/checkpoints/pre_' + cmd[1] + '.tar.gz') + " " + \
                        utilities.absolute_path('data/remote_checkpoints/')))
            c.send('0'.encode())
        elif cmd[0]=='1': # Copy checkpoint data
            print(utilities.run_cmd("sudo -u " + config.user + " rsync -aqz " + config.user + "@" + addr[0] + ":" + \
                    utilities.absolute_path('data/checkpoints/' + cmd[1] + '.tar.gz') + " " + \
                    utilities.absolute_path('data/remote_checkpoints/')))
            c.send('1'.encode())
            #good=0
            #if check_resource(cmd[1],cmd[2],cmd[3]):
            #    c.send('1'.encode())
            #    good=1
            #else:
            #    c.send('t'.encode())
            #utilities.write_log("migration: "+cmd[1]+","+str(good))
        elif cmd[0]=='2': # Restore container
            if config.ARM==0:
                restore_ret = utilities.run_cmd("sudo podman --runtime runc container restore --tcp-established" + \
                        " --import-previous=" + utilities.absolute_path('data/remote_checkpoints/pre_' + cmd[1] + '.tar.gz') + \
                        " --import=" + utilities.absolute_path('data/remote_checkpoints/' + cmd[1] + '.tar.gz'))
            else:
                restore_ret = utilities.run_cmd("sudo podman --runtime runc container restore --tcp-established" + \
                        " --import=" + utilities.absolute_path('data/remote_checkpoints/' + cmd[1] + '.tar.gz'))
            if "that ID is already in use" not in restore_ret:
                utilities.write_log(restore_ret)
                print(utilities.run_cmd("sudo podman start -l"))
                #os.makedirs(utilities.absolute_path('data/containers/'+cmd[1]), exist_ok=True)
                #print(utilities.run_cmd('cp '+utilities.absolute_path('data/nodes.txt')
                #    +' '+utilities.absolute_path('data/containers/'+cmd[1]+'/rank.txt')))
                #os.system('bash ' + absolute_path('launch/run_container_monitor.sh') + ' ' + cmd[1]) # launch container monitor
                c.send('2'.encode())
                utilities.run_cmd(utilities.absolute_path("setup/create_ffiles_container.sh")+ " " + cmd[1])
                with open(utilities.absolute_path("PIController/task_list.txt"), 'a') as the_file:
                    the_file.write(utilities.run_cmd('sudo -E ./PIController/src/get_pid_from_container_id.sh '+cmd[1]+' '+config.cpuaf_tasks))
                m_time = utilities.cycle_counter()
                with open(utilities.absolute_path("data/migration.log"), 'a') as the_file:
                    the_file.write('2,'+str(m_time)+','+cmd[1]+','+addr[0]+'\n')
                utilities.write_log("migration success: "+str(m_time)+","+cmd[1])
            else:
                c.send('t'.encode())

