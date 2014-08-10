#include <iostream>
#include "vsf.h"

using namespace std;

Vsf::Vsf()
{
    func_option_ = FuncOption::get_instance();
    vm_set_ = VmSet::get_instance();
}

Vsf* Vsf::get_instance()
{
    static Vsf vsf;
    return &vsf;
}

//framework
void Vsf::init(std::map<Option, std::map<OptionParam, OptionParamVal> > ops)
{
    static int inited_cnt = 0;
    if(++inited_cnt > 1)
        throw FrameworkInitMoreThanTwice("Vsf::init()");

    func_option_->enable_option(ops);
    return;
}

Host *Vsf::init_host()
{
    host_ = Host::get_instance();
    return host_;
}

set<VM> Vsf::init_vms(Host *host, string vm_cmd)
{
    return vm_set_->init_vms(host, vm_cmd);
}

set<VM> Vsf::init_vms(Host *host)
{
    return vm_set_->init_vms(host);
}
