#include <stdio.h>
#include <stdlib.h>
//#include <iostream>
//#include <fstream>
#include <sstream>
#include <algorithm>
//#include <vector>
//#include <map>
//#include <string>
#include <sys/resource.h>
//#include <unistd.h>
//#include <linux/sched.h>
//#include <sys/syscall.h>
//#include <sys/stat.h>
//#include <memory>
//#include <stdexcept>
//#include <array>
//#include <sys/klog.h>
//#include <dirent.h>
//#include <cstring>

#include "utilities.hpp"
#include "exec_output.hpp"
#include "splitString.hpp"
#include "sched_attr.hpp"
//#include "container_node.hpp"
//#include "RT_tasks.hpp"

#define DO_CONTROL
#define TEST_PARAMETER

/*
struct sched_attr {
    uint32_t size;
    uint32_t sched_policy;
    uint64_t sched_flags;
    int32_t sched_nice;
    uint32_t sched_priority;
    uint64_t sched_runtime;
    uint64_t sched_deadline;
    uint64_t sched_period;
};

struct container_node {
    char name[256];
    int pid;
    float U;
    float M;
    float QoS;
    float period;
};

//struct stat info;

int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags){
    return syscall(SYS_sched_setattr, pid, attr, flags);
}
int sched_getattr(pid_t pid, const struct sched_attr *attr, unsigned int size, unsigned int flags){
    return syscall(SYS_sched_getattr, pid, attr, size, flags);
}
*/

/* constants for PID */
//float Kp_U = 0.185, Kp_M = 0.148;
float Kp_U = 0.17, Kp_M = 0.1;
//const float Ki = 0.01;
//const float Kd = 0.001;
//const int Set_Point = 353;

/* constants for reference */
const float M_s = 0.05; // Miss ratio reference
const float U_s = 0.8; // Utilization reference

const int QoS_levels = 2;
const int which = PRIO_PROCESS;
int cycles;
std::map<int, struct container_node> tasks;
//RT_tasks tasks;
std::map<int, struct container_node>::iterator it;
//RT_tasks_t::iterator it;
float CPU_now = 0.0;
float CPU_now_idle = 0.0;

void AssignQoS(float U, float M, float B_ratio);

/*
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;

    auto pipe = popen(cmd, "r"); // get rid of shared_ptr

    if (!pipe) throw std::runtime_error("popen() failed!");

    while (!feof(pipe)) {
        if (fgets(buffer.data(), 128, pipe) != nullptr)
            result += buffer.data();
    }

    auto rc = pclose(pipe);

    if (rc == EXIT_SUCCESS) { // == 0

    } else if (rc == EXIT_FAILURE) {  // EXIT_FAILURE is not used by all programs, maybe needs some adaptation.

    }
    return result;
}
*/

//void read_pid();

//float read_CPU_Usage();
//float read_Miss_Ratio();

// The feedback control RT controller takes CPU utilization and miss ratio as input
// to calculate the adjust QoS (B) for current cycle. The controller will adjust each
// tasks' attribute if QoS decrease. If the CPU utilization is under threshold,
// we increase each tasks' runtime by New_QoS/Old_QoS. If the miss ratio > 0,
// we decrease each tasks' deadline by New_QoS/Old_QoS.
void Controller()
{
    float U = 0.0, M = 0.0, B = 0.0, old_B = B; // CPU Utilization, Miss ratio, QoS on system level
    float E_M, D_BM, E_U, D_BU, D_B;
    //std::vector<int> tasks;
    std::ofstream report_file;
    //std::vector<std::ofstream> QoS_files;
    std::ofstream QoS_file;
    int cycle_count=0;
    report_file.open("log/report.csv");
    report_file << "second, CPU_Usage, Miss_Count, QoS\n";
    QoS_file.open("log/QoS.csv");
    QoS_file << "second,Container name,QoS\n";

    int ret;
    struct sched_attr attr;

    ret = sched_getattr(0, &attr, sizeof(attr), 0);
    if (ret < 0){
        perror("controller getattr failed.");
        return;
    }

    attr.sched_policy = SCHED_DEADLINE;
    //attr.sched_flags = SCHED_FLAG_DL_OVERRUN;
    attr.sched_runtime = 1000*1000;
    attr.sched_period = 1000*1000*1000;
    attr.sched_deadline = attr.sched_period;

    ret = sched_setattr(0, &attr, 0);
    if (ret < 0){
        perror("controller setattr failed.");
        return;
    }
    //tasks.push_back(0);
    //std::cout << "test" << std::endl;

    while (cycles==0 || cycle_count<cycles) {
	    old_B = B;
        
	    read_pid();
	    /*
	    for(it = tasks.begin(); it != tasks.end(); it++){
	        std::ofstream QoS_file;
	        QoS_file.open("log/" + itoa(it->tid) + "_QoS.csv");
	        QoS_file << "QoS" << std::endl;
	    }
	    */

        U = read_CPU_Usage();
	    //std::cout << "U=" << U << std::endl;
	    M = read_Miss_Ratio();

	    std::cout << "U=" << U << ", M=" << M << std::endl;
#ifdef DO_CONTROL
	    E_M = M_s - M;
        D_BM = Kp_M * E_M;
        E_U = U_s - U;
        D_BU = Kp_U * E_U;
        D_B = std::min(D_BM, D_BU);

	    B = B + D_B;
	    std::cout << "B=" << B << ", old_B=" << old_B << std::endl;
	    if (B<old_B) AssignQoS(U, M, B/old_B);
#endif

        std::string lowest_QoS_name = "";
        float lowest_QoS = 0.0;
	    for(it = tasks.begin(); it != tasks.end(); it++){
            float m = it->second.M;
	        if (m>0) it->second.QoS = it->second.QoS + B/m;
	        //QoS_file << cycle_count << ", " << it->first << ", " << it->second.QoS << std::endl;
	        float qos = it->second.QoS;
            if (qos<lowest_QoS){
                lowest_QoS = qos;
                lowest_QoS_name = it->second.name;
            }
        }
        QoS_file << cycle_count << "," << lowest_QoS_name << "," << lowest_QoS << std::endl;
        report_file << cycle_count << ", " << U*100 << ", " << M << ", " << B << std::endl;
	    cycle_count++;

        sched_yield();
    }
    report_file.close();
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
        /* print all the files and directories within directory *
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
        /* could not open directory *
        perror ("");
        return;
    }
}
#endif

#ifdef TEST_PARAMETER
float read_CPU_Usage()
{
    float U = 0.0, CPU = 0.0, idle = 0.0, used = 0.0;
    unsigned u, n, s, id, io, irq, sirq;
    
    if (std::ifstream("/proc/stat").ignore(33) >> u >> n >> s >> id >> io >> irq >> sirq) {
	std::cout << "u=" << u << ", n=" << n << ", s=" << s << ", id=" << id << ", io=" << io << ", irq=" << irq << ", sirq=" << sirq << std::endl;
        //CPU = (float)(u+n+s+id+io+irq+sirq);
	CPU = (float)(u+n+s+id);
	idle = (float) id;
	used = (CPU - CPU_now) - (idle - CPU_now_idle);
	U = used/(CPU-CPU_now);
	CPU_now = CPU;
	CPU_now_idle = idle;
    }

    return U;
}
#else
float read_CPU_Usage()
{
    std::string s;
    std::vector<std::string> r;
    float U = 0.0;
    std::ifstream fin;
    fin.open("../data/self/stats_processed.csv");
    //std::string s = "../data/containers/";

    if (fin.is_open()){
        fin.seekg(-1,ios_base::end); // go to one spot before the EOF
        fin >> s;
        r = splitString(s, ',');
        U = std::stof(r[1]);
    }
    return U;
}
#endif

std::vector<std::string> splitString(std::string str, char splitter){
    std::vector<std::string> result;
    std::string current = "";
    for(int i = 0; i < (int) str.size(); i++){
        if(str[i] == splitter){
            if(current != ""){
                result.push_back(current);
                current = "";
            }
            continue;
        }
        current += str[i];
    }
    if(current.size() != 0)
        result.push_back(current);
    return result;
}

float read_Miss_Ratio()
{
    float count=0;
    //std::string res = exec("dmesg | grep -i miss 1>&2");
    char t[1];
    int len = klogctl(10, t, 0);
    char res[len];
    std::string token;
    int ret;
    ret = klogctl(4, res, len);
    res[ret] = '\0';
    for (it = tasks.begin(); it != tasks.end(); it++) {
        it->second.M = 0;
    }
    token = res;
    //std::cout << res << std::endl;
    //token = strtok(res, "\n");
    std::vector<std::string> token_split = splitString(token, '\n');
    //while (token!=NULL){
    for (int i = 0; i < (int) token_split.size(); i++) {
	/*
        for (int i=0;i<ret;i++)
	    if (res[i]=='\n') {
                //std::cout << i << std::endl;
	        count++;
	    }
	*
	//std::cout << "token: " << token_split[i] << std::endl;
	//check_miss = strstr(token, "miss");
	//check_miss = strtok(token, " ");
	std::vector<std::string> check_split = splitString(token_split[i], ' ');
	//while (check_split!=NULL) {
	for (int j = 0; j < (int) check_split.size(); j++) {
	    if (check_split[j].find(":")!=std::string::npos) {
                //check_miss[strlen(check_miss)-1] = '\0';
		check_split[j].pop_back();
		//std::cout << "check_miss: " << check_split[j] << std::endl;
		/*
		for (it = tasks.begin(); it != tasks.end(); it++) {
	            std::string path = "/proc/" + std::to_string(it->first) + "/task/" + check_split[j];
		    //std::cout << "path: " << path << std::endl;
		    struct stat info;
		    if (stat(path.c_str(), &info ) == 0) {
		        //std::cout << "path: " << path << std::endl;
			it->second.M++;
		    }
		}
		*
		try{
		    it = tasks.find(stoi(check_split[j]));
		    it->second.M++;
		} catch ( ... ) {
   		    // Deal with the exception
		}
		//std::cout << "tid: " << it->first << ", miss count: " << it->second.M << std::endl;
	        //it = tasks.find(stoi(check_split[j])-1);
		//it->second++;
		//break;
	    }
	    //check_miss = strtok(NULL, " ");
	}
	//std::cout << "check_miss: " << check_miss << std::endl;
	//if (check_miss) count++;
	//token = strtok(NULL, "\n");
    }
    //res = exec("dmesg -c");
    for (it = tasks.begin(); it != tasks.end(); it++) {
	std::cout << "tid: " << it->first << ", miss count: " << it->second.M << std::endl;
	it->second.M = it->second.M / it->second.period;
        count += it->second.M;
    }
    count = count/tasks.size();
    //std::cout << "miss count = " << count << std::endl;
    return count;
}
*/

int main(int argc, char** argv)
{
#ifdef TEST_PARAMETER
    if (argc < 4) {
        std::cerr << "Usage: ./PIController cycles_in_second Kp_U Kp_M" << std::endl;
        return -1;
    }
    Kp_U = atof(argv[2]);
    Kp_M = atof(argv[3]);
#else
    if (argc < 2) {
        std::cerr << "Usage: ./PIController cycles_in_second" << std::endl;
	return -1;
    }
#endif

    cycles = atoi(argv[1]);
    //std::cout << cycles << std::endl;

    Controller();

    return 0;
}

void AssignQoS(float U, float M, float B_ratio)
{
    int ret;
    struct sched_attr attr;
    uint64_t new_deadline;
    uint64_t new_runtime;
    int c = 0;
    std::vector<std::pair<float,int>> items;
    for (it=tasks.begin(); it!=tasks.end(); it++) {
        items.push_back(std::pair<float, int>(it->second.M, it->first));
    }
    std::sort(items.begin(), items.end());
    //std::String out[tasks.size()];
    // Migrated Container first
    if (M>0) {
	//if (B_ratio<0.1) B_ratio = 0.1;
	//if (B_ratio>0.3) B_ratio = 0.3;
	std::cout << "reduce deadline by " << B_ratio << std::endl;
	c = 0;
        for (int i=0;i < (int) items.size();i++) {
	//for (it=tasks.begin(); it!=tasks.end(); it++) {
	    if (c>=5) break;
	    if (items[i].first == 0) continue;
            //if(tasks[i] == ) ret = setpriority(which, pid, -20);
            //else if(tasks[i] == "1") ret = setpriority(which, pid, 0);
            //else ret = setpriority(which, pid, 20);
            ret = sched_getattr(items[i].second, &attr, sizeof(attr), 0);
	    //std::cout << "pid: " << items[i].second << " QoS: " << items[i].first << std::endl;
	    new_deadline = attr.sched_deadline * (1-B_ratio);
	    if (new_deadline > attr.sched_runtime) {
	        attr.sched_deadline = new_deadline;
	        ret = sched_setattr(items[i].second, &attr, 0);
                if (ret < 0){
		    std::cerr << items[i].second << ": setattr failed." << std::endl;
                    //return;
                }
	    }
	    c++;
        }
    }
    else {
        std::cout << "increase runtime by " << B_ratio << std::endl;
        for (int i=0;i<(int)items.size();i++) {
	//for (it=tasks.begin(); it!=tasks.end(); it++) {
            //if(tasks[i] == ) ret = setpriority(which, pid, -20);
            //else if(tasks[i] == "1") ret = setpriority(which, pid, 0);
            //else ret = setpriority(which, pid, 20);
            ret = sched_getattr(items[i].second, &attr, sizeof(attr), 0);
            new_runtime = attr.sched_runtime * (1+B_ratio);
	    if (new_runtime < attr.sched_deadline) {
		attr.sched_runtime = new_runtime;
	        ret = sched_setattr(items[i].second, &attr, 0);
                if (ret < 0){
                    std::cerr << items[i].second << ": setattr failed." << std::endl;
                    //return;
                }
	    }
        }
    }
}
