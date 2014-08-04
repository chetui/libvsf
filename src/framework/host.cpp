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
        map<OptionParam, OptionParamVal> &param = func_option_->get_param(Option::OP_HS_TEST_NODE_DIST);
        if(param.size() == 0)
        {
            node_dist_->refresh_test();
        }
        else
        {
            cout << "ss-1:" << param[OptionParam::PATH].get_string() << endl;
            cout << "ss0:" << param[OptionParam::WORKLOAD_TYPE].get_char() << endl;
            cout << "ss1:" << (MicroWorkloadType)(param[OptionParam::WORKLOAD_TYPE].get_char()) << endl;
            MicroParam p(
                param[OptionParam::PATH].get_string(),
                param[OptionParam::SIZE_IN_MB].get_int(),
                (MicroWorkloadType)(param[OptionParam::WORKLOAD_TYPE].get_char()),
                param[OptionParam::LOOP].get_int()
            );
            node_dist_->refresh_test(p);
        }
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

std::vector<std::vector<int> > Host::test_node_dist()
{
    return node_dist_->get_test_node_dist();
}
int Host::test_node_dist(int node_id_0, int node_id_1)
{
    return node_dist_->get_test_node_dist(node_id_0, node_id_1);
}
