#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>

void write_msg(std::ofstream Info_msg, std::string m)
{
    Info_msg << m << std::endl;
    Info_msg.flush();
}
