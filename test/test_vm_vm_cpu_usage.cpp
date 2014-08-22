#include <unistd.h>
#include <iostream>
#include <set>
#include "gtest/gtest.h"

#include "sysinfo/vm/dynamic/vm_cpu_usage.h"

using namespace std;

class VmCpuUsageTest : public ::testing::Test {
protected:
    VmCpuUsageTest() {
        vm_cpu_usage = VmCpuUsage::get_instance();
    }

    VmCpuUsage* vm_cpu_usage;
};

TEST_F(VmCpuUsageTest, vm_ids_with_thread)
{
    vm_cpu_usage->start();
    sleep(15);
    vm_cpu_usage->stop();
}

//TEST_F(VmCpuUsageTest, vm_ids_without_thread_no_p)
//{
//    vm_base->set_vm_cmd("qemu-system-x86_64");
//    set<VmId> ids = vm_base->get_vm_ids();
//    for(auto& id : ids)
//        cout << id.start_timestamp << ":" << id.pid << " ";
//    cout << endl;
//}
//
//TEST_F(VmCpuUsageTest, vm_ids_without_thread_p)
//{
//    set<VmId> ids = vm_base->get_vm_ids("qemu-system-x86_64");
//    for(auto& id : ids)
//        cout << id.start_timestamp << ":" << id.pid << " ";
//    cout << endl;
//}
//
//TEST_F(VmCpuUsageTest, volatile_vmthread_with_thread)
//{
//    vm_base->set_vm_cmd("qemu-system-x86_64");
//    vm_base->start();
//    set<VmId> ids = vm_base->get_vm_ids();
//    for (auto & id : ids) {
//        cout << "vm volatile_vmthread_num:" << vm_base->get_volatile_vmthread_num(id) << endl;
//        set<pid_t> volatile_vmthread_ids = vm_base->get_volatile_vmthread_ids(id);
//        cout << "vm volatile_vmthread_ids:";
//        for (auto& vid : volatile_vmthread_ids)
//            cout << " " << vid;
//        cout << endl;
//    }
//    vm_base->stop();
//}
//
//    
