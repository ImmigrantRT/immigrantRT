#include <sys/sysinfo.h>

#include "utilities.hpp"
#include "exec_output.hpp"
#include "sched_attr.hpp"
#include "splitString.hpp"

extern std::map<int, struct container_node> tasks;
extern std::map<int, struct container_node>::iterator it;
extern std::string RTCdir;
extern std::ofstream Info_msg;

bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

bool is_float( std::string myString ) {
    std::istringstream iss(myString);
    float f;
    iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
    // Check the entire string was consumed and if either failbit or badbit is set
    return iss.eof() && !iss.fail();
}

void write_msg(std::string m)
{
    Info_msg << m << std::endl;
    Info_msg.flush();
}

/*
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

    write_msg("RTCdir=" + RTCdir + " in read_pid()");
    std::string task_list_path = RTCdir+"/PIController/task_list.txt";
    tasks_file.open(task_list_path.c_str());
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
*/
void read_pid()
{
    //bool err = false;
    int ret;
    struct sched_attr attr;
    std::string container_id = "";

    std::string line;
    std::vector<std::string> r;
    std::ifstream tasks_file;
    int pid, tid;

    for (it = tasks.begin(); it != tasks.end(); it++) it->second.alive = false;

    write_msg("start read_pid()");
    std::string task_list_path = RTCdir+"/PIController/task_list.txt";
    tasks_file.open(task_list_path.c_str());
    while (std::getline(tasks_file, line)) {
	    r = splitString(line, ',');
	    if(r.size()<2) continue;
	    container_id = r[0];
	    if (container_id.length()!=12) {
	        write_msg("error line: container_id=" + line);
		break;
	    }
	    pid = stoi(r[1]);
	    tid = pid;
	    if(r.size()>2) {
	        for(unsigned long i=2;i<r.size();i++) {
		    tid = stoi(r[i]);
		    try{
                    	it = tasks.find(tid);
	            	if (it == tasks.end()){
			    write_msg("sched_getattr before read in read_pid()");
		            ret = sched_getattr(tid, &attr, sizeof(attr), 0);
			    write_msg("sched_getattr after read in read_pid()");
		            if (attr.sched_period != 0 || ret<0) {
			        tasks.insert({tid, {true, container_id, pid, 0.0, 0.0, 0.0, 0.0, 0.0}});
			        tasks[tid].period = ((float) 1000*1000*1000) / attr.sched_period;
		            }
	                }
		        else{
		            it->second.alive = true;
		        }
	            	write_msg("pid=" + std::to_string(pid) + 
				    ", container=" + container_id + 
				    ", tid=" + std::to_string(tid) + " in read_pid()");
		    } catch (...) {
		        write_msg("Fail add: pid=" + std::to_string(pid) +
                                    ", container=" + container_id +
                                    ", tid=" + std::to_string(tid) + " in read_pid()");
		    }
		}
	    }
	    else {
	        try{ 
		    it = tasks.find(tid);
		    if (it == tasks.end()) {
		        ret = sched_getattr(tid, &attr, sizeof(attr), 0);
		        if (attr.sched_period != 0 || ret<0) {
			    tasks.insert({tid, {true, container_id, pid, 0.0, 0.0, 0.0, 0.0, 0.0}});
			    tasks[tid].period = ((float) 1000*1000*1000) / attr.sched_period;
		        }
		    }
		    else{
		        it->second.alive = true;
		    }
		    write_msg("pid=" + std::to_string(pid) + 
				    ", container=" + container_id + 
				    ", tid=" + std::to_string(tid) + " in read_pid()");
		} catch (...) {
		    write_msg("Fail add: pid=" + std::to_string(pid) +
                                    ", container=" + container_id +
                                    ", tid=" + std::to_string(tid) + " in read_pid()");
		}
	    }
    }
    tasks_file.close();
}
//#endif

#ifdef DO_MONITOR
void read_CPU_Usage(std::map<int, struct container_node>::iterator it)
{
    float U = 0.0, CPU = 0.0, idle = 0.0, used = 0.0;
    unsigned u, n, s, id, io, irq, sirq;

    if (std::ifstream("/proc/stat").ignore(33) >> u >> n >> s >> id >> io >> irq >> sirq) {
        write_msg("u=" << std::to_string(u) + ", n=" + std::to_string(n) + ", s=" << std::to_string(s) + ", id=" + std::to_string(id) + ", io=" + std::to_string(io) + ", irq=" + std::to_string(irq) + ", sirq=" << std::to_string(sirq));
        CPU = (float)(u+n+s+id);
        idle = (float) id;
        used = (CPU - CPU_now) - (idle - CPU_now_idle);
        U = used/(CPU-CPU_now);
        CPU_now = CPU;
CPU_now_idle = idle;
    }
    //return U;
}
#else
void read_CPU_Usage(std::map<int, struct container_node>::iterator it)
{
    std::string line,newline;
    std::vector<std::string> r;
    float U = 0.0, M = 0.0;
    std::ifstream fin;
    struct sysinfo info;
    sysinfo(&info);

    write_msg("start read CPU and mem for "+it->second.name+" in read_CPU_Usage()");
    /*
    std::string stats_path = RTCdir+"/data/self/stats.txt";
    write_msg("path=" + stats_path + " in read_CPU_Usage()");
    fin.open(stats_path.c_str());
    while (getline(fin, line)) newline = line;
    r = splitString(newline, ' ');
    r = splitString(r[2], ',');
    U = 100 - std::stof(r[0]);
    fin.close();
    write_msg("system U=" + r[0] + " in read_CPU_Usage()");
    */

    //it = tasks.find(tid);
    //for (it = tasks.begin(); it != tasks.end(); it++) {
    try {
        std::string p(it->second.name);
        p = RTCdir+"/data/containers/"+p+"/stats.txt";
        fin.open(p.c_str());
        newline = "";
        while (getline(fin, line)) newline = line;
	//write_msg("read file in read_CPU_Usage()");
	r = splitString(newline, ' ');
        if (r.size()==3) r = splitString(r[2], ',');
	else {
	    it->second.alive = false;
            return;
	}
	//write_msg("test msg in read_CPU_Usage()");
	//float u = 100 - stof(r[0]);
	if (r.size()==2 && is_float(r[0])){
            U = stof(r[0])/100;
	}
	else {
	    it->second.alive = false;
            return;
	}
	if (is_float(r[1])){
            M = stof(r[1])/info.totalram;
	}
	else {
	    it->second.alive = false;
            return;
	}
	fin.close();
        //it = tasks.find(tid);
        it->second.U = U;
	it->second.Mem = M;
	write_msg("container=" + p + ", U=" + std::to_string(U) + 
			", Mem=" + std::to_string(M) + " in read_CPU_Usage()");
    } catch (...) {
        it->second.alive = false;
	write_msg("fail read CPU and Mem: container=" + it->second.name + 
			" in read_CPU_Usage()");
	return;
    }
    //return 0.0;
}
#endif

float read_Miss_Ratio()
{
    float count=0;
    char t[1];
    int len = klogctl(10, t, 0);
    char res[len];
    std::string token;
    int ret;
    ret = klogctl(4, res, len);
    res[ret] = '\0';
    for (it = tasks.begin(); it != tasks.end(); it++) {
        it->second.Mis= 0;
    }
    token = res;
    std::vector<std::string> token_split = splitString(token, '\n');
    for (int i = 0; i < (int) token_split.size(); i++) {
        std::vector<std::string> check_split = splitString(token_split[i], ' ');
        for (int j = 0; j < (int) check_split.size(); j++) {
            if (check_split[j].find(":")!=std::string::npos) {
                check_split[j].pop_back();
                try{
                    it = tasks.find(stoi(check_split[j]));
                    it->second.Mis++;
                } catch ( ... ) {
                    // Deal with the exception
                }
            }
        }
    }
    for (it = tasks.begin(); it != tasks.end(); it++) {
        write_msg("tid: " + std::to_string(it->first) + ", miss count: " 
			+ std::to_string(it->second.Mis));
        it->second.Mis = it->second.Mis / it->second.period;
        count += it->second.Mis;
    }
    count = count/tasks.size();
    return count;
}
