#include "framework/host.h"

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
            MicroParam p;
            node_dist_->refresh_test(p);
        }
        else
        {
            MicroParam p(
                param[OptionParam::PATH].get_string(),
                param[OptionParam::SIZE_IN_MB].get_int(),
                (MicroWorkloadType)param[OptionParam::WORKLOAD_TYPE].get_char(),
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

int Host::node_num()
{
    return node_core_hpthread_->get_node_num();
}
