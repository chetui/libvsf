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
        { Option::OP_HS_NODE_CPU, { } }, //DONE
        { Option::OP_HS_TOTAL_MEM_SIZE, { } },
        { Option::OP_HS_NODE_SYS_DIST, { } }, //DONE
        { Option::OP_HS_NODE_TEST_DIST, //DONE
            { 
                { OptionParam::PATH, "." },
                { OptionParam::SIZE_IN_MB, 20 },
                { OptionParam::WORKLOAD_TYPE, WORKLOADTYPE_RANDOM },
                { OptionParam::LOOP, 200 }
            }
        },
        //<<Optional Host Dynamic Info>>
        { Option::OP_HS_CPU_USAGE, { } },
        { Option::OP_HS_USED_MEM_SIZE, { } },
        //<<Optional VM Static Info>>
        { Option::OP_VM_VNODE, { } },
        { Option::OP_VM_MEM_POLICY, { } },
        //<<Optional VM Dynamic Info>>
        { Option::OP_VM_BASE,  //DONE
            { 
                { OptionParam::VM_CMD, "qemu-system-x86_64" },
                { OptionParam::LOOP_INTERVAL, 3000 },
                { OptionParam::CALLBACK, VmBaseCallback(vm_base_print_callback) }
            } 
        },
        { Option::OP_VM_CPU_BINDINFO, { } },
        { Option::OP_VM_MEM_BINDINFO, { } },
        { Option::OP_VM_CPU_USAGE, //DONE
            { 
                { OptionParam::LOOP_INTERVAL, 3000 },
                { OptionParam::CALLBACK, VmCpuUsageCallback(vm_cpu_usage_print_callback) }
            } 
        },
        { Option::OP_VM_CACHE_MISS, //DONE
            {
                { OptionParam::LOOP_INTERVAL, 2000 },
                { OptionParam::SAMPLE_INTERVAL, 50000 },
                { OptionParam::CALLBACK, VmCacheMissCallback(vm_cache_miss_print_callback) }
            }
        },
        //{ Option::OP_VM_MEM_SAMPLE, { } },
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

        framework->exec_mig(); //only host parameter may be OK?

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
                //OP_HS_NODE_CPU //Yu
                host->node_num(); //DONE
                host->node_ids(); //DONE
                host->node_id(socket_id); //DONE
                host->node_id(core_id); //DONE //core_id is a struct with 2 int. It is combined with node_id & core_id.
                host->node_id(hpthread_id); //DONE
                host->socket_num(); //DONE
                host->socket_num(node_id); //DONE
                host->socket_ids(); //DONE
                host->socket_ids(node_id); //DONE
                host->socket_id(core_id); //DONE
                host->socket_id(hpthread_id); //DONE
                host->core_num(); //DONE
                host->core_num(node_id); //DONE
                host->core_num(socket_id); //DONE
                host->core_ids(); //DONE
                host->core_ids(node_id); //DONE
                host->core_ids(socket_id); //DONE
                host->core_id(hpthread_id); //DONE
                host->hpthread_num(); //DONE
                host->hpthread_num(node_id); //DONE
                host->hpthread_num(socket_id); //DONE
                host->hpthread_num(core_id); //DONE
                host->hpthread_ids(); //DONE
                host->hpthread_ids(node_id); //DONE
                host->hpthread_ids(socket_id); //DONE
                host->hpthread_ids(core_id); //DONE
                //OP_HS_TOTAL_MEM_SIZE ((( OP_HS_NODE_CPU //Zuo
                host->total_mem_size();
                host->total_mem_size(node_id);
                //OP_HS_NODE_SYS_DIST ((( OP_HS_NODE_CPU //Yu
                host->node_sys_dist(); //DONE
                host->node_sys_dist(node_id_0, node_id_1); //DONE
                //OP_HS_NODE_TEST_DIST ((( OP_HS_NODE_CPU //Yu
                host->node_test_dist(); //DONE
                host->node_test_dist(MicroParam(".", 20, WORKLOADTYPE_RANDOM, 200)); //DONE
                host->node_test_dist(node_id_0, node_id_1); //DONE
                host->node_test_dist(node_id_0, node_id_1, MicroParam(".", 20, WORKLOADTYPE_RANDOM, 200)); //DONE

            //<<host dynamic info>>
                //OP_HS_CPU_USAGE ((( OP_HS_NODE_CPU //Zuo
                host->cpu_usage();
                host->cpu_usage(node_id);
                host->cpu_usage(socket_id);
                host->cpu_usage(core_id);
                host->cpu_usage(hpthread_id);
                //OP_HS_USED_MEM_SIZE ((( OP_HS_NODE_CPU //Zuo
                host->used_mem_size(); 
                host->used_mem_size(node_id);

            //<<VM static info>>
                //OP_VM_VNODE ((( OP_VM_BASE //Zuo
                vm.vcpu_ids(vnode_id); //vNUMA 
                vm.vnode_num(); //vNUMA
                vm.vnode_ids(); //vNUMA //VnodeId need to have a VM start timestamp, because it would be used by set_mem
                //OP_VM_MEM_POLICY ((( OP_HS_NODE_CPU, OP_VM_BASE //Zuo
                vm.mem_policy(); //memory policy is static currently, since it is hard to implement dynamicly
                vm.bindinfo_mem_node_ids();

            //<<VM dynamic info>>
                //OP_VM_BASE //Yu
                vm.vm_id();//DONE
                vm.name(); //DONE
                vm.uuid(); //DONE //no use
                vm.total_mem_size(); //DONE
                    //Host Perspective
                vm.stable_vmthread_num();//DONE //typedef VmthreadId = VmId
                vm.stable_vmthread_ids();//DONE //vcpu_ids + tgid
                vm.volatile_vmthread_num();//DONE //volatile vmthreads would change frequently. Hence, its APIs always execute immediately.
                vm.volatile_vmthread_ids();//DONE
                    //VM Perspective
                vm.vcpu_num();//DONE //Currently not support maxcpus. Throw Exception
                vm.vcpu_ids();//DONE //typedef VcpuId = VmthreadId;
                vm.vsocket_num();//DONE //no use //vsocket,vcore,vhpthread 's id may need vnuma's help
                vm.vcore_num();//DONE //no use
                vm.vhpthread_num();//DONE //no use
                //OP_VM_CPU_BINDINFO ((( OP_HS_NODE_CPU, OP_VM_BASE //Zuo
                vm.bindinfo_hpthread_ids();
                vm.bindinfo_hpthread_ids(vcpu_id/vmthread_id);
                //OP_VM_MEM_BINDINFO ((( OP_VM_VNODE, OP_HS_NODE_CPU, OP_VM_BASE //Zuo
                vm.bindinfo_mem_node_id(vnode_id); //vNUMA
                //OP_VM_CPU_USAGE ((( OP_VM_BASE // Notice! In the non-thread mode, call any function in OP_VM_CPU_USAGE, the cpu_usage is the stat from last calling of any function in OP_VM_CPU_USAGE. In the non-thread mode, the first calling of any function in OP_VM_CPU_USAGE would return 0 or {}; in the thread mode, the calling of any function in OP_VM_CPU_USAGE before the second refresh of thread would return 0 or {}.//Yu 
                vm.sys_cpu_usage(); //DONE //sync with vm.cpu_usage(). Hence, there is a little difference with host->cpu_usage()
                vm.cpu_usage(); //DONE
                vm.cpu_usage(vcpu_id/vmthread_id); //DONE
                vm.running_on_hpthread(vcpu_id/vmthread_id); //DONE
                //OP_VM_CACHE_MISS ((( OP_VM_BASE //Yu
                vm.cache_miss(); //DONE
                vm.cache_miss(vcpu_id/vmthread_id); //DONE
                //OP_VM_MEM_SAMPLE ((( OP_VM_BASE //Yu
                //vm.mem_sample(); //sample the latest visited page addr //have some difficulties to develop on 3.8.0 kernel using perf_event
                //OP_VM_USED_MEM_SIZE ((( OP_HS_NODE_CPU, OP_VM_BASE //Zuo
                vm.used_mem_size();
                vm.used_mem_size(node_id);

        //OUTPUT: decide scheduling strategy

                vm.set_vcpu_mig(vcpu_id/vmthread_id, hpthread_ids); //Yu //DONE
                vm.set_mem_mig(node_ids); //Zuo
                //vm.set_mem_mig(node_ids, addr_start, page_size); //Zuo //depend on OP_VM_MEM_SAMPLE
                vm.set_mem_mig(vnode_id, node_id); //vNUMA //Zuo
    }

    return;
}
