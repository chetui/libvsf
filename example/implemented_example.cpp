#include <iostream>
#include <vector>
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
                { OptionParam::WORKLOAD_TYPE, WORKLOADTYPE_RANDOM },
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

void print_dist(std::vector<std::vector<int> >& dist)
{
    for(std::vector<std::vector<int> >::iterator i = dist.begin(); i != dist.end(); i++)
    {
        for(std::vector<int>::iterator ii = i->begin(); ii != i->end(); ii++)
            std::cout<<*ii<<" ";
        std::cout<<std::endl;
    }
}


void myscheduler(Host *host)
{
    std::cout << "node_num:" << host->node_num() << std::endl;
    std::vector<std::vector<int> > res = host->test_node_dist();
    print_dist(res);
    std::cout << "test_node_dist 0-1: " << host->test_node_dist(0, 1) << std::endl;
    res = host->sys_node_dist();
    print_dist(res);
    std::cout << "sys_node_dist 0-1: " << host->sys_node_dist(0, 1) << std::endl;

    return;
}
