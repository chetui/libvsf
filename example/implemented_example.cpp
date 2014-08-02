#include <iostream>
#include "vsf.h"

void myscheduler();
Vsf* framework = Vsf::get_instance();

int main()
{
    //set some flags to enable optional functions
    framework->set_func_options({
        Option::OP_HW_NODE_CORE_HPTHREAD,
        Option::OP_VM_MISS_RATE,
        Option::OP_VM_MEM_SAMPLE,
    });

    //refresh <<Hardware Static Info>>
    framework->exec_init();
    
    //your scheduler algorithm
    myscheduler();

    return 0;
}

void myscheduler()
{
    std::cout << "node_num:" << framework->node_num() << std::endl;

    return;
}
