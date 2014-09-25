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

int main()
{
    //set some flags ahead of time to refresh optional functions info when init_host(), init_vms(), update_info();
    //if others functions are called without set corresponding flags, info would be collected immediately.
    framework->init({
        { Option::OP_HS_NODE_CPU, { } },
        { Option::OP_HS_NODE_SYS_DIST, { } },
        { Option::OP_HS_NODE_TEST_DIST, 
            { 
                { OptionParam::PATH, "." },
                { OptionParam::SIZE_IN_MB, 20 },
                { OptionParam::WORKLOAD_TYPE, WORKLOADTYPE_RANDOM },
                { OptionParam::LOOP, 200 }
            }
        },
        { Option::OP_VM_BASE,
            {
                { OptionParam::VM_CMD, "qemu-system-x86_64" },
                { OptionParam::LOOP_INTERVAL, 3000 },
                { OptionParam::CALLBACK, VmBaseCallback(vm_base_print_callback) }
            }
        },
        { Option::OP_VM_CPU_USAGE,
            {
                { OptionParam::LOOP_INTERVAL, 3000 }//,
//                { OptionParam::CALLBACK, VmCpuUsageCallback(vm_cpu_usage_print_callback) }
            }
        },
        { Option::OP_VM_CACHE_MISS,
            {
                { OptionParam::LOOP_INTERVAL, 2000 },
                { OptionParam::SAMPLE_INTERVAL, 50000 }//,
//                { OptionParam::CALLBACK, VmCacheMissCallback(vm_cache_miss_print_callback) }
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

        framework->exec_mig();

        print_and_clean_mig(host, vms);

        sleep(1); 

        ++cnt;
    }

    return 0;
}

void print_dist(const std::vector<std::vector<int> >& dist);

void myscheduler(Host *host, std::set<VM> &vms)
{
    //OP_HS_NODE_CPU
    std::cout << "node_num:" << host->node_num() << std::endl;
    std::set<NodeId> node_ids = host->node_ids();
    std::cout << "node_ids:";
    for (auto& id : node_ids)
        std::cout << id << ":";
    std::cout << std::endl;

    std::cout << "socket_num::" << host->socket_num() << std::endl;
    std::set<SocketId> socket_ids = host->socket_ids();
    std::cout << "socket_ids:";
    for (auto& id : socket_ids)
        std::cout << id << ":";
    std::cout << std::endl;

    std::cout << "core_num::" << host->core_num() << std::endl;
    std::set<CoreId> core_ids = host->core_ids();
    std::cout << "core_ids:";
    for (auto& id : core_ids)
        std::cout << id << ":";
    std::cout << std::endl;

    std::cout << "hpthread_num::" << host->hpthread_num() << std::endl;
    std::set<HpthreadId> hpthread_ids = host->hpthread_ids();
    std::cout << "hpthread_ids:";
    for (auto& id : hpthread_ids)
        std::cout << id << ":";
    std::cout << std::endl;

    for (auto& id : socket_ids)
        std::cout << "node_id[socket_id(" << id << ")]:" << host->node_id(id) << std::endl;

    for (auto& id : core_ids)
        std::cout << "node_id[core_id(" << id << ")]:" << host->node_id(id) << std::endl;

    for (auto& id : hpthread_ids)
        std::cout << "node_id[hpthread_id(" << id << ")]:" << host->node_id(id) << std::endl;

    for (auto& id : node_ids)
    {
        std::cout << "socket_num[node_id(" << id << ")]:" << host->socket_num(id) << std::endl;
        std::cout << "socket_ids[node_id(" << id << ")]:";
        std::set<SocketId> ids = host->socket_ids(id);
        for (auto& id2 : ids)
            std::cout << id2 << ":";
        std::cout << std::endl;
    }

    for (auto& id : core_ids)
        std::cout << "socket_id[core_id(" << id << ")]:" << host->socket_id(id) << std::endl;

    for (auto& id : hpthread_ids)
        std::cout << "socket_id[hpthread_id(" << id << ")]:" << host->socket_id(id) << std::endl;

    for (auto& id : node_ids)
    {
        std::cout << "core_num[node_id(" << id << ")]:" << host->core_num(id) << std::endl;
        std::cout << "core_ids[node_id(" << id << ")]:";
        std::set<CoreId> ids = host->core_ids(id);
        for (auto& id2 : ids)
            std::cout << id2 << ":";
        std::cout << std::endl;
    }

    for (auto& id : socket_ids)
    {
        std::cout << "core_num[socket_id(" << id << ")]:" << host->core_num(id) << std::endl;
        std::cout << "core_ids[socket_id(" << id << ")]:";
        std::set<CoreId> ids = host->core_ids(id);
        for (auto& id2 : ids)
            std::cout << id2 << ":";
        std::cout << std::endl;
    }

    for (auto& id : hpthread_ids)
        std::cout << "core_id(hpthread_id(" << id << ")):" << host->core_id(id) << std::endl;

    for (auto& id : node_ids)
    {
        std::cout << "hpthread_num[node_id(" << id << ")]:" << host->hpthread_num(id) << std::endl;
        std::cout << "hpthread_ids[node_id(" << id << ")]:";
        std::set<HpthreadId> ids = host->hpthread_ids(id);
        for (auto& id2 : ids)
            std::cout << id2 << ":";
        std::cout << std::endl;
    }

    for (auto& id : socket_ids)
    {
        std::cout << "hpthread_num[socket_id(" << id << ")]:" << host->hpthread_num(id) << std::endl;
        std::cout << "hpthread_ids[socket_id(" << id << ")]:";
        std::set<HpthreadId> ids = host->hpthread_ids(id);
        for (auto& id2 : ids)
            std::cout << id2 << ":";
        std::cout << std::endl;
    }

    for (auto& id : core_ids)
    {
        std::cout << "hpthread_num[core_id(" << id << ")]:" << host->hpthread_num(id) << std::endl;
        std::cout << "hpthread_ids[core_id(" << id << ")]:";
        std::set<HpthreadId> ids = host->hpthread_ids(id);
        for (auto& id2 : ids)
            std::cout << id2 << ":";
        std::cout << std::endl;
    }

    //OP_HS_NODE_SYS_DIST
    print_dist(host->node_sys_dist());
    std::cout << "node_sys_dist 0-1: " << host->node_sys_dist(0, 1) << std::endl;

    //OP_HS_NODE_TEST_DIST
    print_dist(host->node_test_dist());
    std::cout << "node_test_dist 0-1: " << host->node_test_dist(0, 1) << std::endl;
    framework->set_param({
        { Option::OP_HS_NODE_TEST_DIST, 
            { 
                { OptionParam::PATH, "." },
                { OptionParam::SIZE_IN_MB, 23 },
                { OptionParam::WORKLOAD_TYPE, WORKLOADTYPE_RANDOM },
                { OptionParam::LOOP, 213 }
            }
         }
    });
    print_dist(host->node_test_dist());
    std::cout << "node_test_dist 0-1: " << host->node_test_dist(0, 1) << std::endl;
    framework->clear_param({
        Option::OP_HS_NODE_TEST_DIST
    });

    std::set<HpthreadId> affinity_set = {1, 3, 5};
    for (auto& vm : vms)
    {
        //OP_VM_BASE
        std::cout << "vm_id: " << vm.vm_id() << std::endl;
        std::cout << "name: " << vm.name() << std::endl;
        std::cout << "uuid: " << vm.uuid() << std::endl;
        std::cout << "vsocket_num: " << vm.vsocket_num() << std::endl;
        std::cout << "vcore_num: " << vm.vcore_num() << std::endl;
        std::cout << "vhpthread_num: " << vm.vhpthread_num() << std::endl;
        std::cout << "total_mem_size: " << vm.total_mem_size() << std::endl;
        std::cout << "vcpu: " << vm.vcpu_num() << " ";
        std::set<pid_t> pid_set = vm.vcpu_ids();
        for (auto& pid : pid_set)
            std::cout << "|" << pid;
        std::cout << std::endl;
        std::cout << "stable_vmthread: " << vm.stable_vmthread_num() << " ";
        std::set<pid_t> stable_pid_set = vm.stable_vmthread_ids();
        for (auto& pid : stable_pid_set)
            std::cout << "|" << pid;
        std::cout << std::endl;
        std::cout << "volatile_vmthread: " << vm.volatile_vmthread_num() << " ";
        std::set<pid_t> volatile_pid_set = vm.volatile_vmthread_ids();
        for (auto& pid : volatile_pid_set)
            std::cout << "|" << pid;
        std::cout << std::endl;
        //OP_VM_CPU_USAGE
        std::cout << "sys_cpu_usage:" << vm.sys_cpu_usage() << ":" << std::endl;
        std::cout << "cpu_usage:" << vm.cpu_usage() << std::endl;
        for (auto& pid : stable_pid_set)
            std::cout << "cpu_usage[" << pid << "]:" << vm.cpu_usage(pid) << "[ON]" << vm.running_on_hpthread(pid) << std::endl;
        for (auto& pid : volatile_pid_set)
            std::cout << "cpu_usage[" << pid << "]:" << vm.cpu_usage(pid) << "[ON]" << vm.running_on_hpthread(pid) << std::endl;
        //OP_VM_CACHE_MISS
        std::cout << "cache_miss:" << vm.cache_miss() << std::endl;
        for (auto& pid : stable_pid_set)
            std::cout << "cache_miss[" << pid << "]:" << vm.cache_miss(pid) << std::endl;
        for (auto& pid : volatile_pid_set)
            std::cout << "cache_miss[" << pid << "]:" << vm.cache_miss(pid) << std::endl;
        //CPU MIG
        for (auto& pid : stable_pid_set)
            vm.set_vcpu_mig(pid, affinity_set);
        for (auto& pid : volatile_pid_set)
            vm.set_vcpu_mig(pid, affinity_set);
    }

    framework->clear_param({
        Option::OP_VM_CPU_USAGE,
        Option::OP_VM_CACHE_MISS
    });
    framework->set_param({
//        { Option::OP_VM_BASE,
//            {
//                { OptionParam::VM_CMD, "qemu-system-x86_64" },
//                { OptionParam::LOOP_INTERVAL, 3000 },
//                { OptionParam::CALLBACK, VmBaseCallback(vm_base_print_callback) }
//            }
//        },
        { Option::OP_VM_CPU_USAGE,
            {
                { OptionParam::LOOP_INTERVAL, 2800 },
                { OptionParam::CALLBACK, VmCpuUsageCallback(vm_cpu_usage_print_callback) }
            }
        },
        { Option::OP_VM_CACHE_MISS,
            {
                { OptionParam::LOOP_INTERVAL, 1800 },
                { OptionParam::SAMPLE_INTERVAL, 45000 },
                { OptionParam::CALLBACK, VmCacheMissCallback(vm_cache_miss_print_callback) }
            }
        }
    });

    return;
}

void vm_base_print_callback(
    const VmId& vm_id,
    const std::string& name,
    const std::string& uuid,
    const int vsocket_num,
    const int vcore_num,
    const int vhpthread_num,
    const int total_mem_size,
    const std::set<pid_t>& vcpu_ids,
    const std::set<pid_t>& stable_vmthread_ids
    )
{
    std::cout << "[vm_base]print_callback:" << vm_id << ":"
        << name << ":"
        << uuid << ":"
        << vsocket_num << ":"
        << vcore_num << ":"
        << vhpthread_num << ":"
        << total_mem_size << ":"
        << "[";
    for (auto& id : vcpu_ids)
        std::cout << id << ":"; 
    std::cout << "]:[";
    for (auto& id : stable_vmthread_ids)
        std::cout << id << ":"; 
    std::cout << "]" << std::endl;
}

void vm_cpu_usage_print_callback(pid_t pid, pid_t tid, int cpu_usage)
{
    std::cout << "[vm_cpu_usage]print_callback:" << pid << ":" << tid << ":" << cpu_usage << std::endl;
}

void vm_cache_miss_print_callback(const CacheMissData& data)
{
    std::cout << "[vm_cache_miss]print_callback:" << data << std::endl;
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

void print_and_clean_mig(Host *host, std::set<VM> &vms)
{
    std::set<HpthreadId> all_ids = host->hpthread_ids();
    char buf[10240];
    for (auto& vm : vms) {
        std::cout << "----before clean----" << std::endl;
        std::string cmd = "virsh vcpuinfo " + vm.name() + "| grep 'Affinity'";
        FILE* data = popen(cmd.c_str(), "r");
        while (fgets(buf, sizeof(buf), data)) {
            std::cout << buf;
        }
        pclose(data);

        std::set<pid_t> stable_pid_set = vm.stable_vmthread_ids();
        for (auto& pid : stable_pid_set)
            vm.set_vcpu_mig(pid, all_ids);
        std::set<pid_t> volatile_pid_set = vm.volatile_vmthread_ids();
        for (auto& pid : volatile_pid_set)
            vm.set_vcpu_mig(pid, all_ids);

        framework->exec_mig();
        std::cout << "----after clean----" << std::endl;

        data = popen(cmd.c_str(), "r");
        while (fgets(buf, sizeof(buf), data)) {
            std::cout << buf;
        }
        pclose(data);
    }
}
