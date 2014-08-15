#ifndef _HOST_H_
#define _HOST_H_

#include "utils/func_option.h"
#include "sysinfo/host/static/node_cpu.h"
#include "sysinfo/host/static/node_dist.h"

class Host {
public:
    static Host *get_instance();

    //OP_HS_NODE_CPU
    int node_num();
    std::set<NodeId> node_ids();
    NodeId node_id(SocketId socket_id);
    NodeId node_id(CoreId core_id);
    NodeId node_id(HpthreadId hpthread_id);
    int socket_num();
    int socket_num(NodeId node_id);
    std::set<SocketId> socket_ids();
    std::set<SocketId> socket_ids(NodeId node_id);
    SocketId socket_id(CoreId core_id);
    SocketId socket_id(HpthreadId hpthread_id);
    int core_num();
    int core_num(NodeId node_id);
    int core_num(SocketId socket_id);
    std::set<CoreId> core_ids();
    std::set<CoreId> core_ids(NodeId node_id);
    std::set<CoreId> core_ids(SocketId socket_id);
    CoreId core_id(HpthreadId hpthread_id);
    int hpthread_num();
    int hpthread_num(NodeId node_id);
    int hpthread_num(SocketId socket_id);
    int hpthread_num(CoreId core_id);
    std::set<HpthreadId> hpthread_ids();
    std::set<HpthreadId> hpthread_ids(NodeId node_id);
    std::set<HpthreadId> hpthread_ids(SocketId socket_id);
    std::set<HpthreadId> hpthread_ids(CoreId core_id);
    //OP_HS_SYS_NODE_DIST
    std::vector<std::vector<int> > sys_node_dist();
    int sys_node_dist(int node_id_0, int node_id_1);
    //OP_HS_TEST_NODE_DIST
        //may with Option Parameters. if without Option Parameters, then use default parameters.
    std::vector<std::vector<int> > test_node_dist();
    int test_node_dist(int node_id_0, int node_id_1);
        //must without Option Parameters.
    std::vector<std::vector<int> > test_node_dist(const MicroParam &p);
    int test_node_dist(int node_id_0, int node_id_1, const MicroParam &p);

private:
    Host();

    FuncOption* func_option_;
    NodeCpu* node_cpu_;
    NodeDist* node_dist_;

    //OP_HS_TEST_NODE_DIST
    MicroParam get_param_test_node_dist();

};

#endif
