#include <iostream>
#include "vsf.h"

void myscheduler();
Vsf* framework = Vsf::get_instance();

int main()
{
    //set some flags to enable optional function
    //and refresh <<Hardware Static Info>>
    framework->init({
        Option::OP_NODE_CORE_HPTHREAD,
        Option::OP_MISS_RATE,
        Option::OP_MEM_SAMPLE
    });
    
    //your scheduler algorithm
    myscheduler();

    return 0;
}

void myscheduler()
{
    std::cout << "node_num:" << framework->node_num() << std::endl;

    return;
}
