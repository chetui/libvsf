#include "framework/host.h"

Host::Host()
{
    func_option_ = FuncOption::get_instance();
    node_core_hpthread_ = NodeCoreHpthread::get_instance();
    node_dist_ = NodeDist::get_instance();
}

Host* Host::get_instance()
{
    static Host host;
    return &host;
}

int Host::node_num()
{
    func_option_->check_option(Func::FC_HS_NODE_NUM_P_);
    return node_core_hpthread_->get_node_num();
}
