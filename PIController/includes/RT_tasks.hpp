#include "container_node.hpp"

class RT_tasks
{
    public:
      typedef map<int, struct container_node> RT_tasks_t;

      // get tt by reference
      const RT_tasks_t & get() const {return tt;}
      
      // check if node exist
      bool is_node_find(int tid) {return tt.find(tid)!=tt.end();}
      
      // insert a new node to tt
      void insert_node(int tid, int pid) {tt.insert({tid, {"", pid, 0.0, 0.0, 0.0, 0.0}});}
      
      // set each value in container node
      void set_U(int tid, float u) {tt[tid].U=u;}
      void set_M(int tid, float m) {tt[tid].M=m;}
      void set_QoS(int tid, float qos) {tt[tid].QoS=qos;}
      void set_period(int tid, float p) {tt[tid].period=p;}

      // get each value in container node
      float get_U(int tid) {return tt[tid.U];}
      float get_M(int tid) {return tt[tid.M];}
      float get_QoS(int tid) {return tt[tid.QoS];}
      float get_period(int tid) {return tt[tid.period];}

    private:
      RT_tasks_t tt;
};
