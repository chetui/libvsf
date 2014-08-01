#ifndef _VSF_
#define _VSF_

#include <vector>
#include <initializer_list>
#include "utils/func_option.h"
#include "sysinfo/hardware.h"

class Vm;

class Vsf {
public:
    static Vsf* get_instance();

    //framework
    void init(std::initializer_list<Option> ops);
    std::vector<Vm> vms();
    void update_info(std::vector<Vm> &vms);

    //exec
    void exec_mig();

    //hardware info
    int node_num();

private:
    Vsf();

    Hardware* hardware_;
    FuncOption* func_option_;

};

#endif
