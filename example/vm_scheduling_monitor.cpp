#include <unistd.h>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include "vsf.h"

void myscheduler(Host *host, std::set<VM> &vms);
Vsf* framework = Vsf::get_instance();

int main()
{
    //set some flags ahead of time to refresh optional functions info when init_host(), init_vms(), update_info();
    //if others functions are called without set corresponding flags, info would be collected immediately.
    framework->init({
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
        },
    });

    //refresh <<Optional Host Static Info>>
    Host *host = framework->init_host();
    
    while(true) {

        //refresh <<Optional VM Static Info>>
        //and start threads of <<Optional VM Dynamic Info>>
        std::set<VM> vms = framework->init_vms(host);

        //your scheduler algorithm
        myscheduler(host, vms);

        framework->exec_mig();

        sleep(3); 
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

