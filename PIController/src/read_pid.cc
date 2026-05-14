#include "read_pid.hpp"

#ifdef TEST_PARAMETER
void read_pid()
{  
    DIR *dir;
    int ret;
    struct sched_attr attr;
    std::string PATH = "/proc/";

    std::string line;
    std::ifstream tasks_file;
    int pid, tid;
    tasks_file.open("task_list.txt");
    while (std::getline(tasks_file, line)) {
	    pid = stoi(line);
	    //it = tasks.find(pid);
	    //if (it == tasks.end()){
	    dir = opendir((PATH+line+"/task").c_str());
	    struct dirent *entry = readdir(dir);
	    while (entry != NULL) {
	        if (entry->d_type == DT_DIR) {
		        tid = atoi(entry->d_name);
		        //std::cout << "pid: " << pid << ", tid: " << entry->d_name << std::endl;
		        it = tasks.find(tid);
		        if (it == tasks.end() && tid>0 && tid!=pid){
	                ret = sched_getattr(tid, &attr, sizeof(attr), 0);
		            if (attr.sched_period != 0 || ret<0) {
		                tasks.insert({tid, {"", pid, 0.0, 0.0, 0.0, 0.0}});
		                tasks[tid].period = ((float) 1000*1000*1000) / attr.sched_period;
		            }
		        }
	        }
		    entry = readdir(dir);
	    }
	    closedir(dir);
	//}
    }
    tasks_file.close();
}
#else
void read_pid()
{
    DIR *dir;
    struct dirent *entry;
    int pid;
    int ret;
    struct sched_attr attr;
    char s[] = "sudo podman inspect --format '{{.State.Pid}}' ";

    if ((dir = opendir ("../data/containers/")) != NULL) {
        /* print all the files and directories within directory */
        while ((entry = readdir (dir)) != NULL) {
            //printf ("%s\n", ent->d_name);
            std::string returnCode = exec(strcat(s,entry->d_name));
            pid = std::stoi(returnCode);
            //pid = atoi(strtok(entry->d_name, "_"));
            //std::cout << "pid: " << line << std::endl;
            ret = sched_getattr(pid, &attr, sizeof(attr), 0);
            if (attr.sched_period != 0 || ret<0) {
		        it = tasks.find(pid);
		        if (it == tasks.end()){
                    tasks.insert({pid, {entry->d_name, 0, 0.0, 0.0, 0.0, 0.0}});
                    tasks[pid].period = ((float) 1000*1000*1000) / attr.sched_period;
		        }
            }
        }
        closedir (dir);
    }
    else {
        /* could not open directory */
        perror ("");
        return;
    }
}
#endif
