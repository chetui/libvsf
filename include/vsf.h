#ifndef _VSF_H_
#define _VSF_H_

#include <set>
#include <map>
#include <string>
#include <initializer_list>
#include "utils/func_option.h"
#include "framework/exception.h"
#include "framework/host.h"
#include "framework/vm.h"
#include "framework/exec.h"

class Vsf {
public:
    static Vsf* get_instance();

    //framework
    void init(std::map<Option, std::map<OptionParam, OptionParamVal> > ops);
    void set_param(std::map<Option, std::map<OptionParam, OptionParamVal> > ops);
    void clear_param(std::initializer_list<Option> ops);
    Host *init_host();
    std::set<VM> init_vms(Host *host);
    std::set<VM> init_vms(Host *host, std::string vm_cmd);
    void exec_mig();

private:
    Vsf();

    Host *host_;
    VmSet *vm_set_;
    Exec *exec_;

    FuncOption* func_option_;
};

#endif
