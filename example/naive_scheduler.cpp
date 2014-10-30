#include <iostream>
#include <map>
#include "vsf.h"

Vsf* vsf = Vsf::get_instance();

void myscheduler(Host *host, std::set<VM> &vms)
{
    std::cout << "--------Naive Scheduler--------" << std::endl;

    std::multimap<int, const VM*> sorted_vms;
    for (auto& vm : vms) {
        sorted_vms.emplace(vm.cpu_usage(), &vm);
    }

    std::set<NodeId> node_ids = host->node_ids();
    auto node_id = node_ids.begin();
    for (auto& sorted_vm : sorted_vms) {

        std::cout << "vm_id: " << sorted_vm.second->vm_id();
        std::cout << " cpu_usage: " << sorted_vm.second->cpu_usage();

        if (node_id == node_ids.end()) {
            node_id = node_ids.begin();
        }

        std::cout << " node_id: " << *node_id << std::endl;

        //bind the vm to a node
        for (auto& vm_thread_id : sorted_vm.second->stable_vmthread_ids()) {
            sorted_vm.second->set_vcpu_mig(vm_thread_id, host->hpthread_ids(*node_id));
        }
        ++node_id;
    }

    return;
}

void print_mig_res(std::set<VM> &vms) 
{
    std::cout << "--------Migration Result--------" << std::endl;
    char buf[10240];
    for (auto& vm : vms) {
        std::string cmd = "virsh vcpuinfo " + vm.name() + "| grep 'Affinity'";
        FILE* data = popen(cmd.c_str(), "r");
        while (fgets(buf, sizeof(buf), data)) {
            std::cout << "vm_id: " << vm.vm_id() << " " << buf;
        }
        pclose(data);
    }
}

int main()
{
    vsf->init({
        { Option::OP_HS_NODE_CPU, { } },
        { Option::OP_HS_NODE_SYS_DIST, { } },
        { Option::OP_VM_BASE,
            {
                { OptionParam::VM_CMD, "qemu-system-x86_64" },
                { OptionParam::LOOP_INTERVAL, 3000 }//,
            }
        },
        { Option::OP_VM_CPU_USAGE,
            {
                { OptionParam::LOOP_INTERVAL, 3000 }//,
            }
        }
    });

    //refresh <<Optional Host Static Info>>
    Host *host = vsf->init_host();
    
    while(true) {

        //refresh <<Optional VM Static Info>>
        //and start threads of <<Optional VM Dynamic Info>>
        std::set<VM> vms = vsf->init_vms(host);

        //your scheduler algorithm
        myscheduler(host, vms);

        vsf->exec_mig();

        print_mig_res(vms);

        sleep(1); 

    }

    return 0;
}

