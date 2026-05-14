import os
import yaml

cwd=os.getcwd()
def send_requirement():
    data_containers=os.path.join(cwd,"data/containers/")

    # transmit conatiner initial resoource usage to remote node
    file1=open(os.path.join(cwd,"data/nodes.txt"))
    all_nodes=file1.readlines()
    file1.close()
    #print(all_nodes)

    with open(os.path.join(cwd,"launch/config.yaml"), 'r') as f:
        doc = yaml.safe_load(f)
    user=doc['user']

    for root,dirs,files in os.walk(data_containers):
        #print(dirs)
        for f in files:
            if f=="requirements.txt":
                for n in all_nodes:
                    file2=open(os.path.join(root, f),"r+")
                    line=file2.readline()
                    #print("line1: "+line)
                    ll=line.split(",")
                    line=line.replace(ll[0], n.strip())
                    #print("line2: "+line)
                    file2.write(line)
                    file2.close()
                    os.system("scp -r "+root+" "+user+"@"+n.strip()+":"+data_containers)

if __name__ == "__main__":
    send_requirement()
