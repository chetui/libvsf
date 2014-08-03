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
    return;
}

Host *Vsf::init_host()
{
    host_ = Host::get_instance();
    return host_;
}

