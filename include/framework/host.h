#ifndef _HOST_H_
#define _HOST_H_

#include <map>
#include <vector>
#include "utils/func_option.h"
#include "sysinfo/host/static/node_cpu.h"
#include "sysinfo/host/static/node_sys_dist.h"
#include "sysinfo/host/static/node_test_dist.h"

class Host {
    friend class Vsf;
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
    //OP_HS_NODE_SYS_DIST
    std::vector<std::vector<int> > node_sys_dist();
    int node_sys_dist(int node_id_0, int node_id_1);
    //OP_HS_NODE_TEST_DIST
    std::vector<std::vector<int> > node_test_dist();
    int node_test_dist(int node_id_0, int node_id_1);

private:
    Host();
    void set_param(std::map<Option, std::map<OptionParam, OptionParamVal> > ops);
    void clear_param(std::vector<Option> ops);

    FuncOption* func_option_;
    NodeCpu* node_cpu_;
    NodeSysDist* node_sys_dist_;
    NodeTestDist* node_test_dist_;

    //OP_HS_NODE_TEST_DIST
    MicroParam option_param_to_micro_param(std::map<OptionParam, OptionParamVal>& param);
    std::map<OptionParam, OptionParamVal>  micro_param_to_option_param(MicroParam param);

};

#endif
