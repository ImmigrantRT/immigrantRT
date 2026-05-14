import socket
from socket import AF_INET, SOCK_DGRAM
import os, time, threading, random, argparse, sys

absolute_path = lambda path : os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), path)

def write_log(n, nodes, msg):
    with open(absolute_path("gossip_protocol_test/log/"+str(nodes)+".csv"), 'a') as f:
        f.write(str(n)+','+'-'.join(map(str, msg))+'\n')

def client_handler(n, nodes):
    global q, end, res
    #s = socket.socket()
    target = n
    time.sleep(5)
    #print("test")
    # connect to node and send ready signal
    while q != end:
        try:
            #while (q >> target) & 1:
            #    target = random.randint(0, nodes-1)
            while q[target] == "1":
                target = random.randint(0, nodes-1)
            #print("current_node:" + str(n) + ", target:" + str(target))
            s = socket.socket()
            port = 7000 + target
            s.connect(('localhost', port))
            #print("check")
            s.send("ready".encode())
            get_q = s.recv(1024).decode()
            #print(get_q)
            s.close()
            for i in range(len(q)):
                if q[i]=="0" and get_q[i]=="1":
                    q = q[:i]+"1"+q[i+1:]
            #q = q | get_q
            #print("get_q:"+str(get_q)+", new_q:"+str(q))
            #res.append(get_q.bit_count())
            res.append(get_q.count("1"))
        except socket.error:
            sys.exit()
    #print("client end")

def main():
    parser = argparse.ArgumentParser(description="gossip protocol simulator.")
    parser.add_argument('nodes', metavar='N', type=int, help='total number of nodes')
    parser.add_argument('cn', metavar='C', type=int, help='current node')
    #parser.add_argument('timeout', metavar='T', type=int, help='number of cycles to run')
    args = parser.parse_args()
    nodes = args.nodes
    n = args.cn
    port = 7000+n
    #timeout = args.timeout

    ss = socket.socket()
    ss.bind(('', port))
    ss.listen(5)
    #ss.setblocking(False)
    ss.settimeout(11.0)

    global q, end, res
    #end = (1 << nodes) - 1
    end = "1"*nodes
    #print("end:" + str(end))
    #q = 1 << n
    q = "0"*nodes
    q = q[:n] + '1' + q[n+1:]
    #print("q:",q)
    res = []

    client = threading.Thread(target=client_handler, args=(n,nodes,))
    client.start()

    tt = 0
    while True:
        c = 0
        addr=0
        try:
            c, addr = ss.accept()
            #print(c.recv(1024).decode())
            c.sendall(q.encode())
            ss.settimeout(2.0)
        #except BlockingIOError:
        #    pass
        except socket.timeout:
            ss.close()
            break

    while client.is_alive():
        time.sleep(1)

    write_log(n, nodes, res)
    #print(n,",",q,",",res)

if __name__ == "__main__":
    main()
