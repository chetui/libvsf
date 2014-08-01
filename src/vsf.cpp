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
void Vsf::init(initializer_list<Option> ops) 
{
    func_option_->enable_option(ops);
    return;
}

//hardware info
int Vsf::node_num()
{
    func_option_->check_option(Func::FC_NODE_NUM);
    return hardware_->get_node_num();
}
