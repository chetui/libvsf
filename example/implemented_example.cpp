#include <iostream>
#include "vsf.h"

void myscheduler();
Vsf* framework = Vsf::get_instance();

int main()
{
    //set some flags to enable optional functions
    //and refresh <<Hardware Static Info>>
    framework->init({
        { Option::OP_HW_NODE_CORE_HPTHREAD, { } },
        { Option::OP_VM_MISS_RATE, { } },
        { Option::OP_VM_MEM_SAMPLE, { } },
        { Option::OP_HW_TEST_NODE_DIST, 
            { 
                { OptionParam::PATH, "." },
                { OptionParam::SIZE_IN_MB, 20 },
                { OptionParam::WORKLOAD_TYPE, MWT_RANDOM },
                { OptionParam::LOOP, 200 }
            }
        }
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
