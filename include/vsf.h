#ifndef _VSF_H_
#define _VSF_H_

#include <set>
#include <map>
#include <initializer_list>
#include "utils/func_option.h"
#include "framework/host.h"
#include "framework/vm.h"

class Vsf {
public:
    static Vsf* get_instance();

    //framework
    void init(std::map<Option, std::map<OptionParam, OptionParamVal> > ops);
    Host *init_host();
    std::set<VM> init_vms(Host *host);

    //exec
    void exec_mig();

private:
    Vsf();

    Host *host_;
    VmSet *vm_set_;

    FuncOption* func_option_;
};

#endif
