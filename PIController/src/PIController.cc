#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <algorithm>
#include <sys/resource.h>
#include <cmath>
#include <list>
#include <numeric>

#include "utilities.hpp"
#include "exec_output.hpp"
#include "splitString.hpp"
#include "sched_attr.hpp"
#include "config.hpp"

#define DO_CONTROL
//#define TEST_PARAMETER
//#define DO_MONITOR

/* constants for PID */
//float Kp_U = 0.185, Kp_M = 0.148;
float Kp_U = 0.17, Kp_Mis = 0.1, Kp_Mem = 0.17;
//const float Ki = 0.01;
//const float Kd = 0.001;
//const int Set_Point = 353;

/* constants for reference */
const float Mis_s = 0.05; // Miss ratio reference
float U_s; // CPU Utilization reference
float Mem_s; // Memory Utilization reference

const int QoS_levels = 2;
const int which = PRIO_PROCESS;
int cycles;
std::map<int, struct container_node> tasks;
std::map<int, struct container_node>::iterator it;
std::ofstream Info_msg;
std::string RTCdir(std::getenv("RTCdir"));
float CPU_now = 0.0;
float CPU_now_idle = 0.0;

//void AssignQoS(float U, float M, float B_ratio);
void AssignQoS(float D_BU, std::map<int, struct container_node>::iterator it);

// The feedback control RT controller takes CPU utilization and miss ratio as input
// to calculate the adjust QoS (B) for current cycle. The controller will adjust each
// tasks' attribute if QoS decrease. If the CPU utilization is under threshold,
// we increase each tasks' runtime by New_QoS/Old_QoS. If the miss ratio > 0,
// we decrease each tasks' deadline by New_QoS/Old_QoS.
void Controller(bool do_control)
{
    //float U = 0.0, Mis = 0.0, Mem = 0.0; // CPU Utilization, Miss ratio, Memory Utilization on system level
    float Mis = 0.0;
    float E_Mis, D_BMis, E_U, D_BU, E_Mem, D_BMem;
    float old_QoS;
    std::ofstream report_file;
    //std::vector<std::ofstream> QoS_files;
    std::ofstream QoS_file;
    std::ofstream QoS_log;

    int cycle_count=0;
    //std::string RTCdir(std::getenv("RTCdir"));
    std::string report_path=RTCdir+"/PIController/log/report.csv";
    //std::cout << report_path << std::endl;
    report_file.open(report_path.c_str());
    //std::cout << report_path << std::endl;
    report_file << "second,CPU_Usage,Miss_Count,QoS\n"; report_file.flush();
    //QoS_file.open("log/QoS.csv");
    QoS_file.open((RTCdir+"/data/self/QoS.csv").c_str());
    QoS_file << "second,Container name,QoS\n"; QoS_file.flush();
    Info_msg.open((RTCdir+"/PIController/log/error_msg.txt").c_str(),std::fstream::out);
    QoS_log.open((RTCdir+"/PIController/log/QoS.csv").c_str());

    int ret;
    struct sched_attr attr;

    ret = sched_getattr(0, &attr, sizeof(attr), 0);
    if (ret < 0){
        perror("controller getattr failed.");
        return;
    }

    attr.sched_policy = SCHED_DEADLINE;
    //attr.sched_flags = SCHED_FLAG_DL_OVERRUN;
    attr.sched_runtime = 200*1000*1000;
    attr.sched_period = 1000*1000*1000;
    attr.sched_deadline = attr.sched_runtime;

    ret = sched_setattr(0, &attr, 0);
    if (ret < 0){
        perror("controller setattr failed.");
        return;
    }

    std::list<float> QoSs{};
    auto start = std::chrono::system_clock::now(); // get current time

    while (cycles==0 || cycle_count<cycles) {
        //old_B = B;
        read_pid();
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	write_msg(std::to_string(cycle_count) + ". time=" + 
			std::to_string(elapsed_seconds.count()) + 
			" task.size=" + std::to_string(tasks.size()) + " in Controller()");
	if (tasks.size()>0){
            //U = read_CPU_Usage();
            Mis = read_Miss_Ratio();
            //write_msg("U=" + std::to_string(U) + ", M=" + std::to_string(M));
	    std::string lowest_QoS_name = "";
	    float lowest_QoS = 0.0;
	    for (it=tasks.begin();it!=tasks.end();it++){
	        if (!it->second.alive) continue;
		old_QoS = it->second.QoS;
		U_s = it->second.U;
		Mem_s = it->second.Mem;
		read_CPU_Usage(it);

		write_msg("finish read CPU and Mem in Controller()");
		if (!it->second.alive) continue;
#ifdef DO_CONTROL
    	        E_Mis = Mis_s - it->second.Mis;
                D_BMis = Kp_Mis * E_Mis;
	        E_U = it->second.U - U_s;
                D_BU = Kp_U * E_U;
		E_Mem = it->second.Mem - Mem_s;
		D_BMem = Kp_Mem * E_Mem;
		it->second.QoS = std::min(D_BMis, std::min(D_BU, D_BMem));

                //B = std::min(B, it->second.QoS);
	        //write_msg("B=" + std::to_string(B) + ", old_B=" + std::to_string(old_B));
	        if (do_control && it->second.QoS<old_QoS && it->second.QoS<QOS_THRESHOLD) AssignQoS(D_BU, it);
#endif

	    //for(it = tasks.begin(); it != tasks.end(); it++){
                //float m = it->second.M;
	        //if (m>0) it->second.QoS = it->second.QoS + B*(m-M_s);
	        //QoS_file << cycle_count << ", " << it->first << ", " << it->second.QoS << std::endl;
	        //float qos = it->second.QoS;
		QoS_log << cycle_count << "," << it->second.name 
			<< "," << it->second.QoS << std::endl; QoS_log.flush();
                if (it->second.QoS<lowest_QoS && it->second.QoS<QOS_THRESHOLD){
                    lowest_QoS = it->second.QoS;
                    lowest_QoS_name = it->second.name;
                }
            }
	    if (QoSs.size()>=10) QoSs.pop_front();
	    QoSs.push_back(lowest_QoS);
	    float sum_QoS = std::accumulate(QoSs.begin(), QoSs.end(), 0.0);
	    sum_QoS=1+sum_QoS;
	    if (sum_QoS<0.8){
                QoS_file << cycle_count << "," << lowest_QoS_name 
		    << "," << lowest_QoS << std::endl; QoS_file.flush();
	    }
	    else {
		QoS_file << cycle_count << ",,0" << std::endl; QoS_file.flush();
	    }
	    float oSum = sum_QoS>1?1:(sum_QoS<0?0:sum_QoS);
	    report_file << cycle_count << "," << "," << Mis 
		    //<< "," << lowest_QoS << std::endl; report_file.flush();
		    << "," << oSum << std::endl; report_file.flush();
	}
	cycle_count++;
	sched_yield();
    }
    report_file.close();
    QoS_file.close();
    Info_msg.close();
}

int main(int argc, char** argv)
{
    bool do_control = true;
#ifdef TEST_PARAMETER
    if (argc < 4) {
        std::cerr << "Usage: ./PIController cycles_in_second Kp_U Kp_M" << std::endl;
        return -1;
    }
    Kp_U = atof(argv[2]);
    Kp_M = atof(argv[3]);
#else
    if (argc < 3) {
        std::cerr << "Usage: ./PIController cycles_in_second [do control?1:0]" << std::endl;
	return -1;
    }
    do_control=atoi(argv[2])==1?true:false;
#endif

    cycles = atoi(argv[1]);
    //std::cout << cycles << std::endl;

    sleep(3);
    Controller(do_control);

    return 0;
}

void AssignQoS(float D_BU, std::map<int, struct container_node>::iterator it)
{
    int ret;
    struct sched_attr attr;
    uint64_t new_attr;

    ret = sched_getattr(it->first, &attr, sizeof(attr), 0);
    if (D_BU >= 0) {
        new_attr = attr.sched_runtime * (1-it->second.QoS);
	if (new_attr < attr.sched_deadline) {
	    attr.sched_runtime = new_attr;
	    ret = sched_setattr(it->first, &attr, 0);
	    if (ret < 0){
		    write_msg(std::to_string(it->first) + ": setattr failed.");
	    }
	}
    }
    else {
        new_attr = attr.sched_deadline * (1+it->second.QoS);
        if (new_attr > attr.sched_runtime) {
            attr.sched_deadline = new_attr;
            ret = sched_setattr(it->first, &attr, 0);
	    if (ret < 0){
		    write_msg(std::to_string(it->first) + ": setattr failed.");
	    }
        }
    }
/*
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
	write_msg("reduce deadline by " + std::to_string(B_ratio));
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
		    write_msg(std::to_string(items[i].second) + ": setattr failed.");
                    //return;
                }
	    }
	    c++;
        }
    }
    else {
        write_msg("increase runtime by " + std::to_string(B_ratio));
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
                    write_msg(std::to_string(items[i].second) + ": setattr failed.");
                    //return;
                }
	    }
        }
    }
    */
}
