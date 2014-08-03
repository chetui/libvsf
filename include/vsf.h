#ifndef _VSF_H_
#define _VSF_H_

#include <vector>
#include <map>
#include <initializer_list>
#include "utils/func_option.h"
#include "framework/host.h"

class Vm;

class Vsf {
public:
    static Vsf* get_instance();

    //framework
    void init(std::map<Option, std::map<OptionParam, OptionParamVal> > ops);
    std::vector<Vm> vms();
    Host *init_host();
    std::vector<Vm>& init_vms(Host *host);
    void update_info(std::vector<Vm> &vms);

    //exec
    void exec_mig();

private:
    Vsf();

    Host *host_;

    std::map<Option, std::map<OptionParam, OptionParamVal> > option_param_;
    FuncOption* func_option_;

};

#endif
