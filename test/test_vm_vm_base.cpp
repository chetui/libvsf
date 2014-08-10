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
    
TEST_F(VmBaseTest, vm_total_mem_size_with_thread)
{
    vm_base->set_vm_cmd("qemu-system-x86_64");
    vm_base->start();
    set<VmId> ids = vm_base->get_vm_ids();
    for (auto & id : ids)
        cout << "vm total mem size:" << vm_base->get_vm_total_mem_size(id) << endl;
    vm_base->stop();
}

TEST_F(VmBaseTest, vm_total_mem_size_without_thread)
{
    vm_base->set_vm_cmd("qemu-system-x86_64");
    set<VmId> ids = vm_base->get_vm_ids();
    for (auto & id : ids)
        cout << "vm total mem size:" << vm_base->get_vm_total_mem_size(id) << endl;
}
    
