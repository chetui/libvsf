#include "framework/host.h"
#include <iostream>

using namespace std;

Host::Host()
{
    func_option_ = FuncOption::get_instance();

    node_cpu_ = NodeCpu::get_instance();
    if(func_option_->check_option(Option::OP_HS_NODE_CPU))
        node_cpu_->refresh();

    node_dist_ = NodeDist::get_instance();
    if(func_option_->check_option(Option::OP_HS_SYS_NODE_DIST))
        node_dist_->refresh_sys();
    if(func_option_->check_option(Option::OP_HS_TEST_NODE_DIST))
    {
        node_dist_->refresh_test(get_param_test_node_dist());
    }
}

Host* Host::get_instance()
{
    static Host host;
    return &host;
}

//OP_HS_NODE_CPU
int Host::node_num()
{
    return node_cpu_->get_node_num();
}
std::set<NodeId> Host::node_ids()
{
    return node_cpu_->get_node_ids();
}
NodeId Host::node_id(SocketId socket_id)
{
    return node_cpu_->get_node_id(socket_id);
}
NodeId Host::node_id(CoreId core_id)
{
    return node_cpu_->get_node_id(core_id);
}
NodeId Host::node_id(HpthreadId hpthread_id)
{
    return node_cpu_->get_node_id(hpthread_id);
}
int Host::socket_num()
{
    return node_cpu_->get_socket_num();
}
int Host::socket_num(NodeId node_id)
{
    return node_cpu_->get_socket_num(node_id);
}
std::set<SocketId> Host::socket_ids()
{
    return node_cpu_->get_socket_ids();
}
std::set<SocketId> Host::socket_ids(NodeId node_id)
{
    return node_cpu_->get_socket_ids(node_id);
}
SocketId Host::socket_id(CoreId core_id)
{
    return node_cpu_->get_socket_id(core_id);
}
SocketId Host::socket_id(HpthreadId hpthread_id)
{
    return node_cpu_->get_socket_id(hpthread_id);
}
int Host::core_num()
{
    return node_cpu_->get_core_num();
}
int Host::core_num(NodeId node_id)
{
    return node_cpu_->get_core_num(node_id);
}
int Host::core_num(SocketId socket_id)
{
    return node_cpu_->get_core_num(socket_id);
}
std::set<CoreId> Host::core_ids()
{
    return node_cpu_->get_core_ids();
}
std::set<CoreId> Host::core_ids(NodeId node_id)
{
    return node_cpu_->get_core_ids(node_id);
}
std::set<CoreId> Host::core_ids(SocketId socket_id)
{
    return node_cpu_->get_core_ids(socket_id);
}
CoreId Host::core_id(HpthreadId hpthread_id)
{
    return node_cpu_->get_core_id(hpthread_id);
}
int Host::hpthread_num()
{
    return node_cpu_->get_hpthread_num();
}
int Host::hpthread_num(NodeId node_id)
{
    return node_cpu_->get_hpthread_num(node_id);
}
int Host::hpthread_num(SocketId socket_id)
{
    return node_cpu_->get_hpthread_num(socket_id);
}
int Host::hpthread_num(CoreId core_id)
{
    return node_cpu_->get_hpthread_num(core_id);
}
std::set<HpthreadId> Host::hpthread_ids()
{
    return node_cpu_->get_hpthread_ids();
}
std::set<HpthreadId> Host::hpthread_ids(NodeId node_id)
{
    return node_cpu_->get_hpthread_ids(node_id);
}
std::set<HpthreadId> Host::hpthread_ids(SocketId socket_id)
{
    return node_cpu_->get_hpthread_ids(socket_id);
}
std::set<HpthreadId> Host::hpthread_ids(CoreId core_id)
{
    return node_cpu_->get_hpthread_ids(core_id);
}

//OP_HS_SYS_NODE_DIST
std::vector<std::vector<int> > Host::sys_node_dist()
{
    return node_dist_->get_sys_node_dist();
}

int Host::sys_node_dist(int node_id_0, int node_id_1)
{
    return node_dist_->get_sys_node_dist(node_id_0, node_id_1);
}

MicroParam Host::get_param_test_node_dist()
{
    map<OptionParam, OptionParamVal> &param = func_option_->get_param(Option::OP_HS_TEST_NODE_DIST);
    MicroParam ret_p;
    for (auto & p : param) 
    {
        switch(p.first)
        {
        case OptionParam::PATH:
            ret_p.path = p.second.get_string();
            break;
        case OptionParam::SIZE_IN_MB:
            ret_p.size_in_mb = p.second.get_int();
            break;
        case OptionParam::WORKLOAD_TYPE:
            ret_p.type = p.second.get_char();
            break;
        case OptionParam::LOOP:
            ret_p.loop = p.second.get_int();
            break;
        default:
            //cerr message "invalid parameter for OP_HS_TEST_NODE_DIST"
            break;
        }
    }
    return ret_p;
}

std::vector<std::vector<int> > Host::test_node_dist()
{
    return node_dist_->get_test_node_dist(get_param_test_node_dist());
}

int Host::test_node_dist(int node_id_0, int node_id_1)
{
    return node_dist_->get_test_node_dist(node_id_0, node_id_1, get_param_test_node_dist());
}

std::vector<std::vector<int> > Host::test_node_dist(const MicroParam &p)
{
    return node_dist_->get_test_node_dist(p);
}

int Host::test_node_dist(int node_id_0, int node_id_1, const MicroParam &p)
{
    return node_dist_->get_test_node_dist(node_id_0, node_id_1, p);
}
