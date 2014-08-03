#ifndef _VSF_
#define _VSF_

#include <vector>
#include <map>
#include <initializer_list>
#include "utils/func_option.h"
#include "sysinfo/hardware.h"

class Vm;

class Vsf {
public:
    static Vsf* get_instance();

    //framework
    void init(std::map<Option, std::map<OptionParam, OptionParamVal> > ops);
    std::vector<Vm> vms();
    void update_info(std::vector<Vm> &vms);

    //set parameters
    void set_hw_test_node_dist_param();

    //exec
    void exec_mig();

    //hardware info
    int node_num();

private:
    Vsf();

    std::map<Option, std::map<OptionParam, OptionParamVal> > option_param_;
    Hardware* hardware_;
    FuncOption* func_option_;

};

#endif
