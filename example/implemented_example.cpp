#include <iostream>
#include "vsf.h"

void myscheduler(Host *host);
Vsf* framework = Vsf::get_instance();

int main()
{
    //set some flags ahead of time to refresh optional functions info when init_host(), init_vms(), update_info();
    //if others functions are called without set corresponding flags, info would be collected immediately.
    framework->init({
        { Option::OP_HS_NODE_CORE_HPTHREAD, { } },
        { Option::OP_VM_MISS_RATE, { } },
        { Option::OP_VM_MEM_SAMPLE, { } },
        { Option::OP_HS_TEST_NODE_DIST, 
            { 
                { OptionParam::PATH, "." },
                { OptionParam::SIZE_IN_MB, 20 },
                { OptionParam::WORKLOAD_TYPE, MWT_RANDOM },
                { OptionParam::LOOP, 200 }
            }
        }
    });

    //refresh <<Optional Host Static Info>>
    Host *host = framework->init_host();
    
    //your scheduler algorithm
    myscheduler(host);

    return 0;
}

void myscheduler(Host *host)
{
    std::cout << "node_num:" << host->node_num() << std::endl;

    return;
}
