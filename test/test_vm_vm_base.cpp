#include <unistd.h>
#include <iostream>
#include <set>
#include "gtest/gtest.h"

#include "sysinfo/vm/dynamic/vm_base.h"

using namespace std;

class VmBaseTest : public ::testing::Test {
protected:
    VmBaseTest() {
        vm_base = VmBase::get_instance();
    }

    VmBase* vm_base;
};

TEST_F(VmBaseTest, set_vm_cmd_without_thread)
{
    vm_base->set_vm_cmd("sshd");
    size_t size_before = vm_base->get_vm_ids().size();

    vm_base->set_vm_cmd("bash");
    vm_base->get_vm_ids();

    vm_base->set_vm_cmd("sshd");
    size_t size_after = vm_base->get_vm_ids().size();
    cout << "size:" << size_after << endl;
    ASSERT_EQ(size_before, size_after);
}

TEST_F(VmBaseTest, vm_ids_with_thread)
{
    vm_base->set_vm_cmd("qemu-system-x86_64");
    vm_base->start();
    set<VmId> ids = vm_base->get_vm_ids();
    for(auto& id : ids)
        cout << id.start_timestamp << ":" << id.pid << " ";
    cout << endl;
    vm_base->stop();
}

TEST_F(VmBaseTest, vm_ids_without_thread_no_p)
{
    vm_base->set_vm_cmd("qemu-system-x86_64");
    set<VmId> ids = vm_base->get_vm_ids();
    for(auto& id : ids)
        cout << id.start_timestamp << ":" << id.pid << " ";
    cout << endl;
}

TEST_F(VmBaseTest, vm_ids_without_thread_p)
{
    set<VmId> ids = vm_base->get_vm_ids("qemu-system-x86_64");
    for(auto& id : ids)
        cout << id.start_timestamp << ":" << id.pid << " ";
    cout << endl;
}

TEST_F(VmBaseTest, volatile_vmthread_with_thread)
{
    vm_base->set_vm_cmd("qemu-system-x86_64");
    vm_base->start();
    set<VmId> ids = vm_base->get_vm_ids();
    for (auto & id : ids) {
        cout << "vm volatile_vmthread_num:" << vm_base->get_volatile_vmthread_num(id) << endl;
        set<pid_t> volatile_vmthread_ids = vm_base->get_volatile_vmthread_ids(id);
        cout << "vm volatile_vmthread_ids:";
        for (auto& vid : volatile_vmthread_ids)
            cout << " " << vid;
        cout << endl;
    }
    vm_base->stop();
}

TEST_F(VmBaseTest, volatile_vmthread_without_thread)
{
    vm_base->set_vm_cmd("qemu-system-x86_64");
    set<VmId> ids = vm_base->get_vm_ids();
    for (auto & id : ids)
    {
        cout << "vm volatile_vmthread_num:" << vm_base->get_volatile_vmthread_num(id) << endl;
        set<pid_t> volatile_vmthread_ids = vm_base->get_volatile_vmthread_ids(id);
        cout << "vm volatile_vmthread_ids:";
        for (auto& vid : volatile_vmthread_ids)
            cout << " " << vid;
        cout << endl;
    }
}
    

TEST_F(VmBaseTest, vcpu_ids_with_thread)
{
    vm_base->set_vm_cmd("qemu-system-x86_64");
    vm_base->start();
    set<VmId> ids = vm_base->get_vm_ids();
    for (auto & id : ids) {
        cout << "vm vcpu_num:" << vm_base->get_vcpu_num(id) << endl;
        set<pid_t> vcpu_ids = vm_base->get_vcpu_ids(id);
        cout << "vm vcpu_ids:";
        for (auto& vid : vcpu_ids)
            cout << " " << vid;
        cout << endl;
        cout << "vm stable_vmthread_num:" << vm_base->get_stable_vmthread_num(id) << endl;
        set<pid_t> stable_vmthread_ids = vm_base->get_stable_vmthread_ids(id);
        cout << "vm stable_vmthread_ids:";
        for (auto& vid : stable_vmthread_ids)
            cout << " " << vid;
        cout << endl;
    }
    vm_base->stop();
}

TEST_F(VmBaseTest, vcpu_ids_without_thread)
{
    vm_base->set_vm_cmd("qemu-system-x86_64");
    set<VmId> ids = vm_base->get_vm_ids();
    for (auto & id : ids)
    {
        cout << "vm vcpu_num:" << vm_base->get_vcpu_num(id) << endl;
        set<pid_t> vcpu_ids = vm_base->get_vcpu_ids(id);
        cout << "vm vcpu_ids:";
        for (auto& vid : vcpu_ids)
            cout << " " << vid;
        cout << endl;
        cout << "vm stable_vmthread_num:" << vm_base->get_stable_vmthread_num(id) << endl;
        set<pid_t> stable_vmthread_ids = vm_base->get_stable_vmthread_ids(id);
        cout << "vm stable_vmthread_ids:";
        for (auto& vid : stable_vmthread_ids)
            cout << " " << vid;
        cout << endl;
    }
}
    
    
TEST_F(VmBaseTest, vm_other_info_with_thread)
{
    vm_base->set_vm_cmd("qemu-system-x86_64");
    vm_base->start();
    set<VmId> ids = vm_base->get_vm_ids();
    for (auto & id : ids) {
        cout << "vm name:" << vm_base->get_name(id) << endl;
        cout << "vm uuid:" << vm_base->get_uuid(id) << endl;
        cout << "vm vsocket_num:" << vm_base->get_vsocket_num(id) << endl;
        cout << "vm vcore_num:" << vm_base->get_vcore_num(id) << endl;
        cout << "vm vhpthread:" << vm_base->get_vhpthread_num(id) << endl;
        cout << "vm total mem size:" << vm_base->get_total_mem_size(id) << endl;
    }
    vm_base->stop();
}

TEST_F(VmBaseTest, vm_other_info_without_thread)
{
    vm_base->set_vm_cmd("qemu-system-x86_64");
    set<VmId> ids = vm_base->get_vm_ids();
    for (auto & id : ids)
    {
        cout << "vm name:" << vm_base->get_name(id) << endl;
        cout << "vm uuid:" << vm_base->get_uuid(id) << endl;
        cout << "vm vsocket_num:" << vm_base->get_vsocket_num(id) << endl;
        cout << "vm vcore_num:" << vm_base->get_vcore_num(id) << endl;
        cout << "vm vhpthread:" << vm_base->get_vhpthread_num(id) << endl;
        cout << "vm total mem size:" << vm_base->get_total_mem_size(id) << endl;
    }
}
    
