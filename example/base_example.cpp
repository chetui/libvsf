#include <vector>
#include "vsf.h"

void myscheduler(std::vector<VM> &vms);
Vsf* framework = Vsf::get_instance();

int main()
{
    //set some flags ahead of time to refresh optional functions info when init_host(), init_vms(), update_info();
    //if others functions are called without set corresponding flags, info would be collected immediately.
    framework->init({
        //<<Optional Host Static Info>>
        { Option::OP_HS_NODE_CORE_HPTHREAD, { } },
        { Option::OP_HS_TOTAL_MEM_SIZE, { } },
        { Option::OP_HS_SYS_NODE_DIST, { } },
        { Option::OP_HS_TEST_NODE_DIST, 
            { 
                { OptionParam::PATH, "." },
                { OptionParam::SIZE_IN_MB, 20 },
                { OptionParam::WORKLOAD_TYPE, MWT_RANDOM },
                { OptionParam::LOOP, 200 }
            }
        },
        //<<Optional Host Dynamic Info>>
        { Option::OP_HS_CPU_REUSE_RATIO, { } },
        { Option::OP_HS_CPU_USAGE, { } },
        { Option::OP_HS_USED_MEM_SIZE, { } },
        //<<Optional VM Static Info>>
        { Option::OP_VM_VCPU_VMTHREAD, { } },
        { Option::OP_VM_VNODE, { } },
        { Option::OP_VM_TOTAL_MEM_SIZE, { } },
        { Option::OP_VM_MEM_POLICY, { } },
        //<<Optional VM Dynamic Info>>
        { Option::OP_VM_CPU_BINDINFO, { } },
        { Option::OP_VM_MEM_BINDINFO, { } },
        { Option::OP_VM_CPU_USAGE, { } },
        { Option::OP_VM_MISS_RATE, { } },
        { Option::OP_VM_MEM_SAMPLE, { } },
        { Option::OP_VM_USED_MEM_SIZE { } }
    });

    //refresh <<Optional Host Static Info>>
    Host *host = framework->init_host();
    
    while(1) {

        //refresh <<Optional VM Static Info>>
        std::vector<VM> vms = framework->init_vms(host); //get all the VMs on the host
        //update_info would refresh <<Optional Host & VM Dynamic Info>>
        framework->update_info(host, vms);

        //your scheduler algorithm
        myscheduler(host, vms);

        framework->exec_mig(host, vms); //only host parameter may be OK?

        sleep(1); 
    }

    return 0;
}

void myscheduler(HOST *host, std::vector<VM> &vms)
{
    for (auto& vm : vms)
    {
        //INPUT: available info

            //<<host static info>>
                //OP_HS_NODE_CORE_HPTHREAD //Yu
                host->node_num(); //DONE
                host->node_ids();
                host->node_id(core_id); //core_id is a struct with 2 int. It is combined with node_id & core_id.
                host->node_id(hpthread_id); //DONE
                host->core_num();
                host->core_num(node_id);
                host->core_ids();
                host->core_ids(node_id);
                host->core_id(hpthread_id);
                host->hpthread_num(); //DONE
                host->hpthread_num(node_id); //DONE
                host->hpthread_num(core_id);
                host->hpthread_ids();
                host->hpthread_ids(node_id); //DONE
                host->hpthread_ids(core_id);
                //OP_HS_TOTAL_MEM_SIZE ((( OP_HS_NODE_CORE_HPTHREAD //Zuo
                host->total_mem_size();
                host->total_mem_size(node_id);
                //OP_HS_SYS_NODE_DIST ((( OP_HS_NODE_CORE_HPTHREAD //Yu
                host->sys_node_dist(); //DONE
                host->sys_node_dist(node_id_0, node_id_1); //DONE
                //OP_HS_TEST_NODE_DIST ((( OP_HS_NODE_CORE_HPTHREAD //Yu
                host->test_node_dist(); //DONE
                host->test_node_dist(node_id_0, node_id_1); //DONE

            //<<host dynamic info>>
                //OP_HS_CPU_REUSE_RATIO ((( OP_HS_NODE_CORE_HPTHREAD /Yu
                host->cpu_reuse_ratio();
                host->cpu_reuse_ratio(node_id);
                //OP_HS_CPU_USAGE ((( OP_HS_NODE_CORE_HPTHREAD //Zuo
                host->cpu_usage();
                host->cpu_usage(node_id);
                host->cpu_usage(core_id);
                host->cpu_usage(hpthread_id);
                //OP_HS_USED_MEM_SIZE ((( OP_HS_NODE_CORE_HPTHREAD //Zuo
                host->used_mem_size(); //DONE
                host->used_mem_size(node_id); //DONE

            //<<VM static info>>
                //OP_VM_VCPU_VMTHREAD //Yu
                vm.vm_id();
                vm.vmthread_num();
                vm.vmthread_ids(); //need to check whether some threads would be created, which make vmthread_num & vmthread_ids to be dynamic info.
                vm.vcpu_num();
                vm.vcpu_ids();
                //OP_VM_VNODE ((( OP_VM_VCPU_VMTHREAD //Zuo
                vm.vcpu_ids(vnode_id); //vNUMA //DONE
                vm.vnode_num(); //vNUMA //DONE
                vm.vnode_ids(); //vNUMA //DONE
                //OP_VM_TOTAL_MEM_SIZE //Zuo
                vm.total_mem_size();
                //OP_VM_MEM_POLICY ((( OP_HS_NODE_CORE_HPTHREAD //Zuo
                vm.mem_policy(); //memory policy is static currently, since it is hard to implement dynamicly
                vm.bindinfo_mem_node_ids();

            //<<VM dynamic info>>
                //OP_VM_CPU_BINDINFO ((( OP_VM_VCPU_VMTHREAD, OP_HS_NODE_CORE_HPTHREAD //Zuo
                vm.bindinfo_hpthread_ids();
                vm.bindinfo_hpthread_ids(vcpu_id);
                vm.bindinfo_hpthread_ids(vmthread_id);
                //OP_VM_MEM_BINDINFO ((( OP_VM_VNODE, OP_HS_NODE_CORE_HPTHREAD //Zuo
                vm.bindinfo_mem_node_id(vnode_id); //vNUMA
                //OP_VM_CPU_USAGE ((( OP_VM_VCPU_VMTHREAD //Yu
                vm.cpu_usage();
                vm.cpu_usage(vcpu_id);
                vm.cpu_usage(vmthread_id);
                //OP_VM_MISS_RATE ((( OP_VM_VCPU_VMTHREAD //Yu
                vm.miss_rate(MISS_RATE_TYPE);
                vm.miss_rate(MISS_RATE_TYPE, vcpu_id);
                vm.miss_rate(MISS_RATE_TYPE, vmthread_id);
                //OP_VM_MEM_SAMPLE //Yu
                vm.mem_sample(); //sample the latest visited page addr
                //OP_VM_USED_MEM_SIZE ((( OP_HS_NODE_CORE_HPTHREAD //Zuo
                vm.used_mem_size();
                vm.used_mem_size(node_id);

        //OUTPUT: decide scheduling strategy

                framework->set_vcpu_mig(vcpu_id, hpthread_ids); //Yu
                framework->set_vcpu_mig(vmthread_id, hpthread_ids); //Yu
                framework->set_mem_mig(vm_id, node_ids); //Zuo
                framework->set_mem_mig(vm_id, node_ids, addr_start, page_size); //Zuo
                framework->set_mem_mig(vnode_id, node_id); //vNUMA //Zuo
    }

    return;
}
