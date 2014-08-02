#include <vector>
#include "vsf.h"

void myscheduler(std::vector<VM> &vms);
Vsf* framework = Vsf::get_instance();

int main()
{
    //set some flags to enable optional functions
    framework->set_func_options({
        //<<hardware static info>>
        Option::OP_HW_NODE_CORE_HPTHREAD,
        Option::OP_HW_TOTAL_MEM_SIZE,
        Option::OP_HW_SYS_NODE_DIST,
        Option::OP_HW_TEST_NODE_DIST,
        //<<hardware dynamic info>>
        Option::OP_HW_CPU_REUSE_RATIO,
        Option::OP_HW_CPU_USAGE,
        Option::OP_HW_USED_MEM_SIZE,
        //<<vm static info>>
        Option::OP_VM_VCPU_VMTHREAD,
        Option::OP_VM_VNODE,
        Option::OP_VM_TOTAL_MEM_SIZE,
        Option::OP_VM_MEM_POLICY,
        //<<vm dynamic info>>
        Option::OP_VM_CPU_BINDINFO,
        Option::OP_VM_MEM_BINDINFO,
        Option::OP_VM_CPU_USAGE,
        Option::OP_VM_MISS_RATE,
        Option::OP_VM_MEM_SAMPLE,
        Option::OP_VM_USED_MEM_SIZE
    });
    //set parameters for some optional functions. if do not set parameters, default parameters would be used.
    framework->set_hw_test_node_dist_param(".", 20, MicroType::SERIAL, 20);

    //refresh <<Hardware Static Info>>
    framework->exec_init();
    
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
                //OP_HW_NODE_CORE_HPTHREAD
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
                //OP_HW_TOTAL_MEM_SIZE
                framework->total_mem_size();
                //OP_HW_TOTAL_MEM_SIZE, OP_HW_NODE_CORE_HPTHREAD
                framework->total_mem_size(node_id);
                //OP_HW_SYS_NODE_DIST, OP_HW_NODE_CORE_HPTHREAD
                framework->sys_node_dist(); //DONE
                framework->sys_node_dist(node_id_0, node_id_1); //DONE
                //OP_HW_TEST_NODE_DIST, OP_HW_NODE_CORE_HPTHREAD
                framework->test_node_dist(); //DONE
                framework->test_node_dist(node_id_0, node_id_1); //DONE

            //<<hardware dynamic info>>
                //OP_HW_CPU_REUSE_RATIO
                framework->cpu_reuse_ratio();
                //OP_HW_CPU_REUSE_RATIO, OP_HW_NODE_CORE_HPTHREAD
                framework->cpu_reuse_ratio(node_id);
                //OP_HW_CPU_USAGE
                framework->cpu_usage();
                //OP_HW_CPU_USAGE, OP_HW_NODE_CORE_HPTHREAD
                framework->cpu_usage(node_id);
                framework->cpu_usage(core_id);
                framework->cpu_usage(hpthread_id);
                //OP_HW_USED_MEM_SIZE
                framework->used_mem_size(); //DONE Zuo
                //OP_HW_USED_MEM_SIZE, OP_HW_NODE_CORE_HPTHREAD
                framework->used_mem_size(node_id); //DONE Zuo

            //<<VM static info>>
                //OP_VM_VCPU_VMTHREAD
                vm.vm_id();
                vm.vmthread_num();
                vm.vmthread_ids(); //need to check whether some threads would be created, which make vmthread_num & vmthread_ids to be dynamic info.
                vm.vcpu_num();
                vm.vcpu_ids();
                //OP_VM_VCPU_VMTHREAD, OP_VM_VNODE
                vm.vcpu_ids(vnode_id); //vNUMA //DONE Zuo
                //OP_VM_VNODE
                vm.vnode_num(); //vNUMA //DONE Zuo
                vm.vnode_ids(); //vNUMA //DONE Zuo
                //OP_VM_TOTAL_MEM_SIZE
                vm.total_mem_size();
                //OP_VM_MEM_POLICY
                vm.mem_policy(); //memory policy is static currently, since it is hard to implement dynamicly
                //OP_VM_MEM_POLICY, OP_HW_NODE_CORE_HPTHREAD
                vm.bindinfo_mem_node_ids();

            //<<VM dynamic info>>
                //OP_VM_CPU_BINDINFO, OP_VM_VCPU_VMTHREAD, OP_HW_NODE_CORE_HPTHREAD
                vm.bindinfo_hpthread_ids();
                vm.bindinfo_hpthread_ids(vcpu_id);
                vm.bindinfo_hpthread_ids(vmthread_id);
                //OP_VM_MEM_BINDINFO, OP_VM_VNODE, OP_HW_NODE_CORE_HPTHREAD
                vm.bindinfo_mem_node_id(vnode_id); //vNUMA
                //OP_VM_CPU_USAGE
                vm.cpu_usage();
                //OP_VM_CPU_USAGE, OP_VM_VCPU_VMTHREAD
                vm.cpu_usage(vcpu_id);
                vm.cpu_usage(vmthread_id);
                //OP_VM_MISS_RATE
                vm.miss_rate(MISS_RATE_TYPE);
                //OP_VM_MISS_RATE, OP_VM_VCPU_VMTHREAD
                vm.miss_rate(MISS_RATE_TYPE, vcpu_id);
                vm.miss_rate(MISS_RATE_TYPE, vmthread_id);
                //OP_VM_MEM_SAMPLE
                vm.mem_sample(); //sample the latest visited page addr
                //OP_VM_USED_MEM_SIZE
                vm.used_mem_size();
                //OP_VM_USED_MEM_SIZE, OP_HW_NODE_CORE_HPTHREAD
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
