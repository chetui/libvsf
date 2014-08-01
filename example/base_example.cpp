#include <vector>
#include "vsf.h"

void myscheduler(std::vector<VM> &vms);
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
    
    while(1) {

        //refresh <<VM Static Info>>
        std::vector<VM> vms = framework->vms();
        //update_info would refresh <<Hardware & VM Dynamic Info>>
        framework->update_info(vms);

        //your scheduler algorithm
        myscheduler(vms);

        framework->exec_mig();

        sleep(1); 
    }

    return 0;
}

void myscheduler(std::vector<VM> &vms)
{
    for (auto& vm : vms)
    {
        //INPUT: available info

            //<<hardware static info>>
                framework->node_num(); //DONE
                framework->node_ids();
                framework->node_id(core_id); //core_id is a struct with 2 int. It is combined with node_id & core_id.
                framework->node_id(hpthread_id); //DONE
                framework->core_num();
                framework->core_num(node_id);
                framework->core_ids();
                framework->core_ids(node_id);
                framework->core_id(hpthread_id);
                framework->hpthread_num(); //DONE
                framework->hpthread_num(node_id); //DONE
                framework->hpthread_num(core_id);
                framework->hpthread_ids();
                framework->hpthread_ids(node_id); //DONE
                framework->hpthread_ids(core_id);
                framework->total_mem_size();
                framework->total_mem_size(node_id);
                framework->node_dist(); //DONE
                framework->node_dist(node_id_0, node_id_1); //DONE

            //<<hardware dynamic info>>
                framework->cpu_reuse_ratio();
                framework->cpu_reuse_ratio(node_id);
                framework->cpu_usage();
                framework->cpu_usage(node_id);
                framework->cpu_usage(core_id);
                framework->cpu_usage(hpthread_id);
                framework->used_mem_size(); //DONE Zuo
                framework->used_mem_size(node_id); //DONE Zuo

            //<<VM static info>>
                vm.vm_id();
                vm.vnode_num(); //vNUMA //DONE Zuo
                vm.vnode_ids(); //vNUMA //DONE Zuo
                vm.vcpu_num();
                vm.vcpu_ids();
                vm.vcpu_ids(vnode_id); //vNUMA //DONE Zuo
                vm.vmthread_num();
                vm.vmthread_ids(); //need to check whether some threads would be created, which make vmthread_num & vmthread_ids to be dynamic info.
                vm.total_mem_size();
                vm.mem_policy(); //memory policy is static currently, since it is hard to implement dynamicly
                vm.bindinfo_mem_node_ids();

            //<<VM dynamic info>>
                vm.bindinfo_mem_node_id(vnode_id); //vNUMA
                vm.bindinfo_hpthread_ids();
                vm.bindinfo_hpthread_ids(vcpu_id) 
                vm.bindinfo_hpthread_ids(vmthread_id) 
                vm.cpu_usage();
                vm.cpu_usage(vcpu_id);
                vm.cpu_usage(vmthread_id);
                vm.miss_rate(MISS_RATE_TYPE);
                vm.miss_rate(MISS_RATE_TYPE, vcpu_id);
                vm.miss_rate(MISS_RATE_TYPE, vmthread_id);
                vm.mem_sample(); //sample the latest visited page addr
                vm.used_mem_size();
                vm.used_mem_size(node_id);

        //OUTPUT: decide scheduling strategy

                framework->set_vcpu_mig(vcpu_id, hpthread_ids);
                framework->set_vcpu_mig(vmthread_id, hpthread_ids);
                framework->set_mem_mig(vm_id, node_ids);
                framework->set_mem_mig(vm_id, node_ids, addr_start, page_size);
                framework->set_mem_mig(vnode_id, node_id); //vNUMA
    }

    return;
}
