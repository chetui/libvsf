#include <unistd.h>
#include <iostream>
#include <set>
#include <vector>
#include "vsf.h"

void myscheduler(Host *host, std::set<VM> &vms);
Vsf* framework = Vsf::get_instance();

int main()
{
    //set some flags ahead of time to refresh optional functions info when init_host(), init_vms(), update_info();
    //if others functions are called without set corresponding flags, info would be collected immediately.
    framework->init({
        { Option::OP_HS_NODE_CORE_HPTHREAD, { } },
        { Option::OP_HS_TEST_NODE_DIST, 
            { 
                { OptionParam::PATH, "." },
                { OptionParam::SIZE_IN_MB, 20 },
                { OptionParam::WORKLOAD_TYPE, WORKLOADTYPE_RANDOM },
                { OptionParam::LOOP, 200 }
            }
        },
        { Option::OP_VM_BASE,
            {
                { OptionParam::VM_CMD, "qemu-system-x86_64" }
            }
        }
    });

    //refresh <<Optional Host Static Info>>
    Host *host = framework->init_host();
    
    int cnt = 0;
    while(cnt < 2) {

        //refresh <<Optional VM Static Info>>
        //and start threads of <<Optional VM Dynamic Info>>
        //std::set<VM> vms = framework->init_vms(host);
        std::set<VM> vms = framework->init_vms(host, "qemu-system-x86_64");

        //your scheduler algorithm
        myscheduler(host, vms);

        sleep(1); 

        ++cnt;
    }

    return 0;
}

void print_dist(const std::vector<std::vector<int> >& dist);

void myscheduler(Host *host, std::set<VM> &vms)
{
    std::cout << "node_num:" << host->node_num() << std::endl;

    print_dist(host->sys_node_dist());
    std::cout << "sys_node_dist 0-1: " << host->sys_node_dist(0, 1) << std::endl;

    print_dist(host->test_node_dist());
    print_dist(host->test_node_dist(MicroParam(".", 23, WORKLOADTYPE_RANDOM, 213)));
    std::cout << "test_node_dist 0-1: " << host->test_node_dist(0, 1) << std::endl;
    std::cout << "test_node_dist 0-1 with p: " << host->test_node_dist(0, 1, MicroParam(".", 23, WORKLOADTYPE_RANDOM, 213)) << std::endl;

    for (auto& vm : vms)
    {
        std::cout << "vm_id: " << vm.vm_id() << std::endl;
        std::cout << "total_mem_size: " << vm.total_mem_size() << std::endl;
    }

    return;
}

void print_dist(const std::vector<std::vector<int> >& dist)
{
    for (const auto& col : dist)
    {
        for (const auto& row : col)
            std::cout << row << " ";
        std::cout << std::endl;
    }

}

