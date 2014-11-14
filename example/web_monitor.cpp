#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <set>
#include <string>
#include <vector>
#include <sstream>
#include "vsf.h"

Vsf* framework = Vsf::get_instance();

void print_dist(const std::vector<std::vector<int> >& dist)
{
    for (const auto& col : dist)
    {
        for (const auto& row : col)
            std::cout << row << " ";
        std::cout << std::endl;
    }
}

void get_vcpu_affinity(std::string& result, std::string vm_name)
{
    FILE *in;
    char buf[10240];

    std::string cmd = "./get_vcpu_affinity.py " + vm_name;
    if (!(in = popen(cmd.c_str(), "r"))) {
        exit(1);
    }

    std::stringstream ss;
    
    while (fgets(buf, sizeof(buf), in) != NULL) {
        ss << buf;
    }
    pclose(in);

    result = ss.str();
}

void get_hpthreads_string(std::string& result, Host *host)
{
    std::stringstream ss;
    ss << "[";
    std::set<NodeId> node_ids = host->node_ids();
    int count = 0;
    for(auto& nid: node_ids)
    {
        // ',' problem
        if(count != 0)
            ss << ",";
        count++;
 
        std::set<HpthreadId> hpthread_ids = host->hpthread_ids(nid);
        ss << "{\"nodeid\":";
        ss << nid;
        ss << ", \"hpthreads\": [";

        int hp_count = 0;
        for(auto& tid: hpthread_ids)
        {
            if(hp_count != 0)
                ss << ",";
            hp_count++;

            ss << tid;
        }
        ss << "]}";
    }
    ss << "]";
    result = ss.str();
}
void get_vms_string(std::string& result, std::set<VM> &vms)
{
    std::stringstream ss;
    ss << "[";

    int count = 0;
    for (auto& vm : vms)
    {
        if(count != 0)
            ss << ",";
        count++;
        
        ss << "{";

        ss << "\"vmid\": \"";
        ss << vm.vm_id();
        ss << "\", \"vmname\": \"";
        ss << vm.name();
        ss << "\", \"vcpus\": [";

        std::set<pid_t> pid_set = vm.vcpu_ids();
        int vcpu_count = 0;
        for (auto& pid : pid_set)
        {
            if(vcpu_count != 0)
                ss << ",";
            vcpu_count++;
            
            ss << "{\"vcpuid\": ";
            ss << pid;
            ss << ", \"runningon\": ";
            ss << vm.running_on_hpthread(pid);
            ss << ", \"usage\": ";
            ss << vm.cpu_usage(pid);
            ss << "}";
        }
        ss << "], \"affinity\": ";
        std::string affinity_string;
        get_vcpu_affinity(affinity_string, vm.name());
        ss << affinity_string;
        ss << "}";
                
    }

    ss << "]";

    result = ss.str();
    
}
void myscheduler(Host *host, std::set<VM> &vms)
{
    std::string data_nodes;
    std::string data_links;

    std::string hpthreads_string;
    get_hpthreads_string(hpthreads_string, host);
    std::cout << hpthreads_string << std::endl;

    std::string vms_string;
    get_vms_string(vms_string, vms);
    std::cout << vms_string << std::endl;

    //node distances
    print_dist(host->node_sys_dist());
    //cpu usage
    std::cout << "HOST CPU usage: " << vms.begin()->sys_cpu_usage() << std::endl; 
    for (auto& vm : vms) {
        std::cout << "VM: " << vm.name() << " CPU usage: " << vm.cpu_usage() << std::endl;
    }

    //cpu topology
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

    //memory usage
    //TODO
    //disk usage
    //TODO
    //network usage
    //TODO
    
    return;
}

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
    
    while (true) {
        //refresh <<Optional VM Static Info>>
        //and start threads of <<Optional VM Dynamic Info>>
        std::set<VM> vms = framework->init_vms(host);

        //your scheduler algorithm
        myscheduler(host, vms);

    }

    return 0;
}

