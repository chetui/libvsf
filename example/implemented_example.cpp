#include <unistd.h>
#include <iostream>
#include <set>
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
                { OptionParam::VM_CMD, "qemu-system-x86_64" }
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

    std::cout << "node_ids:";
    for (auto& id : host->node_ids())
        std::cout << id << ":";
    std::cout << std::endl;

    std::cout << "node_id(socket_id(0)):" << host->node_id(SocketId(0));
    std::cout << "node_id(core_id(1, 10)):" << host->node_id(CoreId(1, 10));
    std::cout << "node_id(hpthread(14)):" << host->node_id(HpthreadId(14));

//    std::cout << "node_id:" << host->socket_num(), 2);
//    std::cout << "node_id:" << host->socket_num(NodeId(0)), 1);
//    
//    res = host->socket_ids() == std::set<SocketId>{ SocketId(0), SocketId(1) };
//    for (auto& id : host->socket_ids())
//        cout << id << ":";
//    cout << endl;
//    ASSERT_EQ(res, true);
//
//    res = host->socket_ids(NodeId(1)) == std::set<SocketId>{ SocketId(0) };
//    for (auto& id : host->socket_ids(NodeId(1)))
//        cout << id << ":";
//    cout << endl;
//    ASSERT_EQ(res, true);
//
//    ASSERT_EQ(host->socket_id(CoreId(1, 10)), SocketId(1));
//    ASSERT_EQ(host->socket_id(HpthreadId(9)), SocketId(0));
//
//    ASSERT_EQ(host->core_num(), 8);
//    ASSERT_EQ(host->core_num(NodeId(1)), 4);
//    ASSERT_EQ(host->core_num(SocketId(1)), 4);
//
//    res = host->core_ids() == std::set<CoreId>{ CoreId(0,0), CoreId(0,1), CoreId(0, 9), CoreId(0, 10), CoreId(1, 0), CoreId(1, 1), CoreId(1, 9), CoreId(1, 10) };
//    for (auto& id : host->core_ids())
//        cout << id << ":";
//    cout << endl;
//    ASSERT_EQ(res, true);
//
//    res = host->core_ids(NodeId(1)) == std::set<CoreId>{ CoreId(0,0), CoreId(0,1), CoreId(0, 9), CoreId(0, 10) };
//    for (auto& id : host->core_ids(NodeId(1)))
//        cout << id << ":";
//    cout << endl;
//    ASSERT_EQ(res, true);
//
//    res = host->core_ids(SocketId(0)) == std::set<CoreId>{ CoreId(0,0), CoreId(0,1), CoreId(0, 9), CoreId(0, 10) };
//    for (auto& id : host->core_ids(SocketId(0)))
//        cout << id << ":";
//    cout << endl;
//    ASSERT_EQ(res, true);
//
//    ASSERT_EQ(host->core_id(HpthreadId(4)), CoreId(1, 9));
//
//    ASSERT_EQ(host->hpthread_num(), 16);
//    ASSERT_EQ(host->hpthread_num(NodeId(1)), 8);
//    ASSERT_EQ(host->hpthread_num(SocketId(1)), 8);
//    ASSERT_EQ(host->hpthread_num(CoreId(0, 9)), 2);
//
//    res = host->hpthread_ids() == std::set<HpthreadId>{ HpthreadId(0), HpthreadId(1), HpthreadId(2), HpthreadId(3), HpthreadId(4), HpthreadId(5), HpthreadId(6), HpthreadId(7), HpthreadId(8), HpthreadId(9), HpthreadId(10), HpthreadId(11), HpthreadId(12), HpthreadId(13), HpthreadId(14), HpthreadId(15) };
//    for (auto& id : host->hpthread_ids())
//        cout << id << ":";
//    cout << endl;
//    ASSERT_EQ(res, true);
//
//    res = host->hpthread_ids(NodeId(0)) == std::set<HpthreadId>{ HpthreadId(0), HpthreadId(2), HpthreadId(4), HpthreadId(6), HpthreadId(8), HpthreadId(10), HpthreadId(12), HpthreadId(14) };
//    for (auto& id : host->hpthread_ids(NodeId(0)))
//        cout << id << ":";
//    cout << endl;
//    ASSERT_EQ(res, true);
//
//    res = host->hpthread_ids(SocketId(1)) == std::set<HpthreadId>{ HpthreadId(0), HpthreadId(2), HpthreadId(4), HpthreadId(6), HpthreadId(8), HpthreadId(10), HpthreadId(12), HpthreadId(14) };
//    for (auto& id : host->hpthread_ids(SocketId(1)))
//        cout << id << ":";
//    cout << endl;
//    ASSERT_EQ(res, true);
//
//    res = host->hpthread_ids(CoreId(0, 9)) == std::set<HpthreadId>{ HpthreadId(5), HpthreadId(13) };
//    for (auto& id : host->hpthread_ids(CoreId(0, 9)))
//        cout << id << ":";
//    cout << endl;
//    ASSERT_EQ(res, true);

    //OP_HS_SYS_NODE_DIST
    print_dist(host->sys_node_dist());
    std::cout << "sys_node_dist 0-1: " << host->sys_node_dist(0, 1) << std::endl;

    //OP_HS_TEST_NODE_DIST
    print_dist(host->test_node_dist());
    print_dist(host->test_node_dist(MicroParam(".", 23, WORKLOADTYPE_RANDOM, 213)));
    std::cout << "test_node_dist 0-1: " << host->test_node_dist(0, 1) << std::endl;
    std::cout << "test_node_dist 0-1 with p: " << host->test_node_dist(0, 1, MicroParam(".", 23, WORKLOADTYPE_RANDOM, 213)) << std::endl;

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
        pid_set = vm.stable_vmthread_ids();
        for (auto& pid : pid_set)
            std::cout << "|" << pid;
        std::cout << std::endl;
        std::cout << "volatile_vmthread: " << vm.volatile_vmthread_num() << " ";
        pid_set = vm.volatile_vmthread_ids();
        for (auto& pid : pid_set)
            std::cout << "|" << pid;
        std::cout << std::endl;
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

