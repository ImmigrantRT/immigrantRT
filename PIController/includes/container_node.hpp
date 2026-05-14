#include <string>

struct container_node {
    bool alive;
    std::string name;
    int pid;
    float U;   // CPU usage
    float Mis; // miss deadline rate
    float Mem; // memory usage
    float QoS;
    float period;
};
