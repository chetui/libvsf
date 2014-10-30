#include <unistd.h>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include "vsf.h"

void myscheduler(Host *host, std::set<VM> &vms);
Vsf* framework = Vsf::get_instance();
void print_and_clean_mig(Host *host, std::set<VM> &vms);
void vm_base_print_callback( const VmId& vm_id, const std::string& name, const std::string& uuid, const int vsocket_num, const int vcore_num, const int vhpthread_num, const int total_mem_size, const std::set<pid_t>& vcpu_ids, const std::set<pid_t>& stable_vmthread_ids);
void vm_cpu_usage_print_callback(pid_t pid, pid_t tid, int cpu_usage);
void vm_cache_miss_print_callback(const CacheMissData& data);
void myscheduler(Host *host, std::set<VM> &vms)
{
    // fill your scheduling algorithm here
    // you can bound some vcpus to hyperthread or set its affinity
}
   
int main()
{
    //set some flags ahead of time to refresh optional functions info when init_host(), init_vms(), update_info();
    //if others functions are called without set corresponding flags, info would be collected immediately.
    framework->init({

});
Host *host = framework->init_host();
    
    while(1) {

        //refresh <<Optional VM Static Info>>
        //and start threads of <<Optional VM Dynamic Info>>
        std::set<VM> vms = framework->init_vms(host);

        //your scheduler algorithm
        myscheduler(host, vms);

        framework->exec_mig(); //only host parameter may be OK?

        sleep(1); 
    }

    return 0;
}
