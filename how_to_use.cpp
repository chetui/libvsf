#include <vector>
#include "framework.h"

Framework framework;

int main()
{
    //set some flags to enable optional function
    //and refresh Hardware Static Info
    framework.init(ACCU_NODE_DIST | MISS_RATE | MEM_ADDR | MEM_DISTR);
    
    while(1) {

        //refresh VM Static Info
        std::vector<VM> vms = framework.vms();
        //update_info would refresh Hardware & VM Dynamic Info
        framework.update_info(vms);

        //your scheduler algorithm
        myscheduler(vms);

        framework.exec_mig();

        sleep(1); 
    }
}

void myscheduler(std::vector<VM> &vms)
{
    for (vm : vms)
    {
        //INPUT: available info

            //hardware

                //static
                framework.node_num();
                framework.node_ids();
                framework.cpu_num();
                framework.cpu_num(nodeid);
                framework.cpu_ids();
                framework.cpu_ids(nodeid);
                framework.total_mem_size();
                framework.total_mem_size(nodeid);
                framework.node_dist();
                framework.node_dist(nodeid0, nodeid1);

                //dynamic
                framework.cpu_reuse_ratio();
                framework.cpu_reuse_ratio(nodeid);
                framework.used_mem_size();
                framework.used_mem_size(nodeid);

            //VM

                //static
                //vm.vnode_num(); //vNUMA
                //vm.vnode_ids(); //vNUMA
                vm.vcpu_num();
                //vm.vcpu_ids(); //vNUMA
                //vm.vcpu_ids(vnodeid); //vNUMA
                vm.total_mem_size();
                vm.mem_mode(); //memory mode is static currently, since it is hard to implement dynamicly
                vm.bound_mem_node_ids();

                //dynamic
                //vm.bound_node(vnodeid); //vNUMA
                //vm.bound_cpu_ids(vcpuid) //vNUMA
                vm.bound_cpu_ids();
                vm.cpu_usage();
                vm.miss_rate(MISS_RATE_TYPE);
                vm.mem_sample(); //sample the latest visited page addr
                vm.used_mem_size();
                vm.used_mem_size(nodeid);

        //OUTPUT: decide scheduling strategy

                //framework.set_vcpu_mig(vcpu_id, cpu_ids); //vNUMA
                framework.set_vcpu_mig(vm, vcpu_node_ids);
                framework.set_mem_mig(vm, mem_node_ids);
                framework.set_mem_mig(vm, mem_node_ids[vm], mem_addr_start, mem_page_size);
    }
}
