#include "framework/host.h"
#include <iostream>

using namespace std;

Host::Host()
{
    func_option_ = FuncOption::get_instance();

    node_core_hpthread_ = NodeCoreHpthread::get_instance();
    if(func_option_->check_option(Option::OP_HS_NODE_CORE_HPTHREAD))
        node_core_hpthread_->refresh();

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

//OP_HS_NODE_CORE_HPTHREAD
int Host::node_num()
{
    return node_core_hpthread_->get_node_num();
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
