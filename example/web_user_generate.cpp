#include "vsf.h"

#include <iostream>
#include <map>
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
int main()
{
    //set some flags ahead of time to refresh optional functions info when init_host(), init_vms(), update_info();
    //if others functions are called without set corresponding flags, info would be collected immediately.
    vsf->init({

{ Option::OP_HS_NODE_CPU, { } },
{ Option::OP_HS_CPU_USAGE, { } },
{ Option::OP_VM_BASE, {
	{ OptionParam::VM_CMD, "qemu-system-x86_64"},
	{ OptionParam::LOOP_INTERVAL,3000}
}}
,
{ Option::OP_VM_CPU_USAGE, {
	{ OptionParam::LOOP_INTERVAL,3000}
}}

});
    //refresh <<Optional Host Static Info>>
    Host *host = vsf->init_host();
    
    while(true) {

        //refresh <<Optional VM Static Info>>
        //and start threads of <<Optional VM Dynamic Info>>
        std::set<VM> vms = vsf->init_vms(host);

        //your scheduler algorithm
        myscheduler(host, vms);

        vsf->exec_mig(); //only host parameter may be OK?

        sleep(1); 
    }

    return 0;
}
