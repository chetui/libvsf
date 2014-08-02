#include "vsf.h"

using namespace std;

Vsf::Vsf()
{
    hardware_ = Hardware::get_instance();
    func_option_ = FuncOption::get_instance();
}

Vsf* Vsf::get_instance()
{
    static Vsf vsf;
    return &vsf;
}

//framework
void Vsf::set_func_options(initializer_list<Option> ops) 
{
    func_option_->enable_option(ops);
    return;
}

void Vsf::exec_init()
{
    hardware_->refresh();
}

//hardware info
int Vsf::node_num()
{
    func_option_->check_option(Func::FC_HW_NODE_NUM_P_);
    return hardware_->get_node_num();
}
