#ifndef _VSF_H_
#define _VSF_H_

#include <set>
#include <map>
#include <string>
#include <stdexcept>
#include <initializer_list>
#include "utils/func_option.h"
#include "framework/host.h"
#include "framework/vm.h"

class FrameworkInitMoreThanTwice : public std::logic_error 
{
public:
    explicit FrameworkInitMoreThanTwice(const std::string &str):
        std::logic_error(
            "Framework can not be initialized more than twice by function " + str
        ) 
    {}
};

class Vsf {
public:
    static Vsf* get_instance();

    //framework
    void init(std::map<Option, std::map<OptionParam, OptionParamVal> > ops);
    Host *init_host();
    std::set<VM> init_vms(Host *host);
    std::set<VM> init_vms(Host *host, std::string vm_cmd);

    //exec
    void exec_mig();

private:
    Vsf();

    Host *host_;
    VmSet *vm_set_;

    FuncOption* func_option_;
};

#endif
