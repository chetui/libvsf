#include <iostream>
#include "vsf.h"

using namespace std;

Vsf::Vsf()
{
    func_option_ = FuncOption::get_instance();
}

Vsf* Vsf::get_instance()
{
    static Vsf vsf;
    return &vsf;
}

//framework
void Vsf::init(std::map<Option, std::map<OptionParam, OptionParamVal> > ops)
{
    option_param_ = ops;
    for (auto& op : ops) 
        func_option_->enable_option({op.first});
//    cout << option_param_[Option::OP_HW_TEST_NODE_DIST][OptionParam::LOOP].get_int() << endl;
    hardware_ = Hardware::get_instance();
    return;
}

//hardware info
int Vsf::node_num()
{
    func_option_->check_option(Func::FC_HW_NODE_NUM_P_);
    return hardware_->get_node_num();
}
