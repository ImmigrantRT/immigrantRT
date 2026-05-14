#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sys/klog.h>
#include <dirent.h>
#include <cstring>
#include <array>
#include <boost/process.hpp>
#include <sstream>

//#include "exec_output.hpp"
//#include "sched_attr.hpp"
#include "container_node.hpp"
//#include "splitString.hpp"
#include "config.hpp"

namespace bp = boost::process;

//typedef map<int, struct container_node> RT_tasks_t;

//std::map<int, struct container_node> tasks;
//std::map<int, struct container_node>::iterator it;

bool is_number(const std::string& s);
bool is_float( std::string myString );
void write_msg(std::string m);
void read_pid();
void read_CPU_Usage(std::map<int, struct container_node>::iterator it);
float read_Miss_Ratio();
