# Usage: python3 client.py [r|m] container_name
# r: copy pre-checkpoint regularly
# m: do migration

# Import socket module
import socket
from socket import AF_INET,SOCK_DGRAM
import sys
import os
import threading
import select
import yaml

file_path = os.path.realpath(__file__)
absolute_path = lambda path : os.path.join(os.path.dirname(file_path), path)

with open(absolute_path('../launch/config.yaml'), 'r') as f:
    data = f.read().replace('\t', '  ')
    config = yaml.load(data, yaml.SafeLoader)
    user = config['user']

# Create a socket object
#s = socket.socket(AF_INET,SOCK_DGRAM)        
#s = socket.socket()

# Define the port on which you want to connect
port = 12345               

# get the IP addresses of destination nodes
def get_ip_addresses(file):
    with open(file, 'r') as f:
        ip_addresses = f.read().splitlines()
    
    return ip_addresses

if sys.argv[1] == 'r':
    lines = get_ip_addresses(absolute_path('../data/nodes.txt'))
else:
    lines = get_ip_addresses(absolute_path('../data/containers/' + sys.argv[2] + '/rank.txt'))
 
# create global variable to make sure only one destination restore the container
#global res
#res = 0

# creating a lock
lock = threading.Lock()

# read argv
c = sys.argv[1]
c_name = sys.argv[2]

# do pre-checkpoint
os.system("sudo podman container checkpoint -P -e " + 
          absolute_path('../data/checkpoints/pre_' + c_name + '.tar.gz') + 
          " " + c_name)
os.system("sudo chown " + user + " " + 
          absolute_path('../data/checkpoints/pre_' + c_name + '.tar.gz'))

def connection_handler(line, c, c_name, lock):
    global res
    res = 0
    # Create a socket object
    #s = socket.socket(AF_INET,SOCK_DGRAM)
    #print(c_name)
    s = socket.socket()
    s.connect((line, port))
    #s.settimeout(10.0)
    s.send(("0,"+c_name).encode())
    if s.recv(1024).decode()=='0':
        if c=='m':
            os.system("sudo podman container checkpoint --with-previous -e " + 
                      absolute_path('../data/checkpoints/' + c_name + '.tar.gz --tcp-established') +
                      " " + c_name)
            os.system("sudo chown " + user + " " + 
                      absolute_path('../data/checkpoints/' + c_name + '.tar.gz'))
            s.send(("1,"+c_name).encode())
            if s.recv(1024).decode()=='1':
                lock.acquire()
                if res==0:
                    s.send(("2,"+c_name).encode())
                    if s.recv(1024).decode()=='2':
                        os.system("sudo podman rm "+c_name)
                        res = 1
                lock.release()
        s.send(("3,"+c_name).encode())
    s.close()

# connect to the server on local computer
for line in lines:
    #s.connect((line, port))
    threading.Thread(target=connection_handler, args=(line,c,c_name,lock,)).start()
#s.connect(("128.105.145.200", port))
#threading.Thread(target=connection_handler, args=(s,c,c_name,lock,)).start()

#data = "podman --runtime runc container restore --tcp-established --import-previous=pre-checkpoint_ws.tar.gz --import=checkpoint_ws.tar.gz"
#s.send(data.encode())

#print (s.recv(1024).decode())
# close the connection
#s.close() 
