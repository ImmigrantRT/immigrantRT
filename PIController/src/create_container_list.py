import os

cwd=os.environ['RTCdir']
data_containers=os.path.join(cwd,"data/containers/")

# store container id and pid to task_list.txt
if os.path.isfile(os.path.join(cwd,"PIController/task_list.txt")):
    os.remove(os.path.join(cwd,"PIController/task_list.txt"))
tasks = open(os.path.join(cwd,"PIController/task_list.txt"), "w")
for container in os.listdir(data_containers):
    pid = os.popen("sudo podman inspect --format '{{.State.Pid}}' "+container).read()
    print(container+" "+pid)
    tasks.write(container+" "+pid)
tasks.close()
