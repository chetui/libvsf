#include <unistd.h>
#include <set>
#include "vsf.h"

void myscheduler(Host *host, std::set<VM> &vms);
Vsf* framework = Vsf::get_instance();

int main()
{
    //set some flags to refresh optional info ahead of time.
    //for optional static info, they would be collected when init_host() or init_vms() is called; 
    //for optional dynamic info, their collecting threads would be started when init_host() or init_vms() is called. 
    //the corresponding functions of optional info can be called with almost no cost;
    //but if other functions are called without set corresponding flags, info would be collected immediately. It would have some performance cost depended on the collecting action/
    framework->init({
        //<<Optional Host Static Info>>
        { Option::OP_HS_NODE_CORE_HPTHREAD, { } },
        { Option::OP_HS_TOTAL_MEM_SIZE, { } },
        { Option::OP_HS_SYS_NODE_DIST, { } },
        { Option::OP_HS_TEST_NODE_DIST, 
            { 
                { OptionParam::PATH, "." },
                { OptionParam::SIZE_IN_MB, 20 },
                { OptionParam::WORKLOAD_TYPE, WORKLOADTYPE_RANDOM },
                { OptionParam::LOOP, 200 }
            }
        },
        //<<Optional Host Dynamic Info>>
        { Option::OP_HS_CPU_REUSE_RATIO, { } },
        { Option::OP_HS_CPU_USAGE, { } },
        { Option::OP_HS_USED_MEM_SIZE, { } },
        //<<Optional VM Static Info>>
        { Option::OP_VM_VNODE, { } },
        { Option::OP_VM_MEM_POLICY, { } },
        //<<Optional VM Dynamic Info>>
        { Option::OP_VM_BASE, 
            { 
                { OptionParam::VM_CMD, "qemu-system-x86_64" }
            } 
        },
        { Option::OP_VM_CPU_BINDINFO, { } },
        { Option::OP_VM_MEM_BINDINFO, { } },
        { Option::OP_VM_CPU_USAGE, { } },
        { Option::OP_VM_MISS_RATE, { } },
        { Option::OP_VM_MEM_SAMPLE, { } },
        { Option::OP_VM_USED_MEM_SIZE { } }
    });

    //refresh <<Optional Host Static Info>>,
    //and start threads of <<Optional Host Dynamic Info>>
    Host *host = framework->init_host();
    
    while(1) {

        //refresh <<Optional VM Static Info>>
        //and start threads of <<Optional VM Dynamic Info>>
        std::set<VM> vms = framework->init_vms(host);
        std::set<VM> vms = framework->init_vms(host, "qemu-system-x86_64");

        //your scheduler algorithm
        myscheduler(host, vms);

        framework->exec_mig(host, vms); //only host parameter may be OK?

        sleep(1); 
    }

    return 0;
}

void myscheduler(HOST *host, std::set<VM> &vms)
{
    for (auto& vm : vms)
    {
        //INPUT: available info

            //<<host static info>>
                //OP_HS_NODE_CORE_HPTHREAD //Yu
                host->node_num(); //DONE
                host->node_ids();
                host->node_id(socket_id);
                host->node_id(core_id); //core_id is a struct with 2 int. It is combined with node_id & core_id.
                host->node_id(hpthread_id);
                host->socket_num();
                host->socket_num(node_id);
                host->socket_ids();
                host->socket_ids(node_id);
                host->socket_id(core_id);
                host->socket_id(hpthread_id);
                host->core_num();
                host->core_num(node_id);
                host->core_ids();
                host->core_ids(node_id);
                host->core_ids(socket_id);
                host->core_id(hpthread_id);
                host->hpthread_num();
                host->hpthread_num(node_id);
                host->hpthread_num(core_id);
                host->hpthread_ids();
                host->hpthread_ids(node_id);
                host->hpthread_ids(socket_id);
                host->hpthread_ids(core_id);
                //OP_HS_TOTAL_MEM_SIZE ((( OP_HS_NODE_CORE_HPTHREAD //Zuo
                host->total_mem_size();
                host->total_mem_size(node_id);
                //OP_HS_SYS_NODE_DIST ((( OP_HS_NODE_CORE_HPTHREAD //Yu
                host->sys_node_dist(); //DONE
                host->sys_node_dist(node_id_0, node_id_1); //DONE
                //OP_HS_TEST_NODE_DIST ((( OP_HS_NODE_CORE_HPTHREAD //Yu
                host->test_node_dist(); //DONE
                host->test_node_dist(MicroParam(".", 20, WORKLOADTYPE_RANDOM, 200)); //DONE
                host->test_node_dist(node_id_0, node_id_1); //DONE
                host->test_node_dist(node_id_0, node_id_1, MicroParam(".", 20, WORKLOADTYPE_RANDOM, 200)); //DONE

            //<<host dynamic info>>
                //OP_HS_CPU_REUSE_RATIO ((( OP_HS_NODE_CORE_HPTHREAD /Yu
                host->cpu_reuse_ratio();
                host->cpu_reuse_ratio(node_id);
                host->cpu_reuse_ratio(socket_id);
                //OP_HS_CPU_USAGE ((( OP_HS_NODE_CORE_HPTHREAD //Zuo
                host->cpu_usage();
                host->cpu_usage(node_id);
                host->cpu_usage(socket_id);
                host->cpu_usage(core_id);
                host->cpu_usage(hpthread_id);
                //OP_HS_USED_MEM_SIZE ((( OP_HS_NODE_CORE_HPTHREAD //Zuo
                host->used_mem_size(); 
                host->used_mem_size(node_id);

            //<<VM static info>>
                //OP_VM_VNODE ((( OP_VM_BASE //Zuo
                vm.vcpu_ids(vnode_id); //vNUMA 
                vm.vnode_num(); //vNUMA
                vm.vnode_ids(); //vNUMA //VnodeId need to have a VM start timestamp, because it would be used by set_mem
                //OP_VM_MEM_POLICY ((( OP_HS_NODE_CORE_HPTHREAD, OP_VM_BASE //Zuo
                vm.mem_policy(); //memory policy is static currently, since it is hard to implement dynamicly
                vm.bindinfo_mem_node_ids();

            //<<VM dynamic info>>
                //OP_VM_BASE //Yu
                vm.vm_id();//DONE
                vm.name();//no use //DONE
                vm.uuid();//no use //DONE
                vm.total_mem_size(); //DONE
                    //Host Perspective
                vm.stable_vmthread_num();//typedef VmthreadId = VmId
                vm.stable_vmthread_ids();//vcpu_ids + tgid
                vm.volatile_vmthread_num();//volatile vmthreads would change frequently. Hence, its APIs always execute immediately.
                vm.volatile_vmthread_ids();
                    //VM Perspective
                vm.vcpu_num();//Currently not support maxcpus. Throw Exception
                vm.vcpu_ids();//typedef VcpuId = VmthreadId;
                vm.vsocket_num();//no use //vsocket,vcore,vhpthread 's id may need vnuma's help //DONE
                vm.vcore_num();//no use //DONE
                vm.vhpthread_num();//no use //DONE
                //OP_VM_CPU_BINDINFO ((( OP_HS_NODE_CORE_HPTHREAD, OP_VM_BASE //Zuo
                vm.bindinfo_hpthread_ids();
                vm.bindinfo_hpthread_ids(vcpu_id/vmthread_id);
                //OP_VM_MEM_BINDINFO ((( OP_VM_VNODE, OP_HS_NODE_CORE_HPTHREAD, OP_VM_BASE //Zuo
                vm.bindinfo_mem_node_id(vnode_id); //vNUMA
                //OP_VM_CPU_USAGE ((( OP_VM_BASE //Yu
                vm.cpu_usage();
                vm.cpu_usage(vcpu_id/vmthread_id);
                //OP_VM_MISS_RATE ((( OP_VM_BASE //Yu
                vm.miss_rate(MISS_RATE_TYPE);
                vm.miss_rate(MISS_RATE_TYPE, vcpu_id/vmthread_id);
                //OP_VM_MEM_SAMPLE ((( OP_VM_BASE //Yu
                vm.mem_sample(); //sample the latest visited page addr
                //OP_VM_USED_MEM_SIZE ((( OP_HS_NODE_CORE_HPTHREAD, OP_VM_BASE //Zuo
                vm.used_mem_size();
                vm.used_mem_size(node_id);

        //OUTPUT: decide scheduling strategy

                framework->set_vcpu_mig(vcpu_id/vmthread_id, hpthread_ids); //Yu
                framework->set_mem_mig(vm_id, node_ids); //Zuo
                framework->set_mem_mig(vm_id, node_ids, addr_start, page_size); //Zuo
                //which one??
                framework->set_mem_mig(vm_id, vnode_id, node_id); //vNUMA //Zuo
                //framework->set_mem_mig(vnode_id, node_id); //vNUMA //Zuo
    }

    return;
}
