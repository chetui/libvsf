#include <unistd.h>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include "vsf.h"

void myscheduler(Host *host, std::set<VM> &vms);
Vsf* framework = Vsf::get_instance();
void print_and_clean_mig(Host *host, std::set<VM> &vms);

int main()
{
    //set some flags ahead of time to refresh optional functions info when init_host(), init_vms(), update_info();
    //if others functions are called without set corresponding flags, info would be collected immediately.
    framework->init({
        { Option::OP_HS_NODE_CPU, { } },
        { Option::OP_HS_SYS_NODE_DIST, { } },
        { Option::OP_HS_TEST_NODE_DIST, 
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
                { OptionParam::LOOP_INTERVAL, 3000 }
            }
        },
        { Option::OP_VM_CPU_USAGE,
            {
                { OptionParam::LOOP_INTERVAL, 3000 }
            }
        },
        { Option::OP_VM_CACHE_MISS,
            {
                { OptionParam::LOOP_INTERVAL, 2000 },
                { OptionParam::SAMPLE_INTERVAL, 50000 }
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

    //OP_HS_SYS_NODE_DIST
    print_dist(host->sys_node_dist());
    std::cout << "sys_node_dist 0-1: " << host->sys_node_dist(0, 1) << std::endl;

    //OP_HS_TEST_NODE_DIST
    print_dist(host->test_node_dist());
    print_dist(host->test_node_dist(MicroParam(".", 23, WORKLOADTYPE_RANDOM, 213)));
    std::cout << "test_node_dist 0-1: " << host->test_node_dist(0, 1) << std::endl;
    std::cout << "test_node_dist 0-1 with p: " << host->test_node_dist(0, 1, MicroParam(".", 23, WORKLOADTYPE_RANDOM, 213)) << std::endl;

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

    return;
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
