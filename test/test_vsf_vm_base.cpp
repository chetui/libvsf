#include <iostream>
#include <set>
#include "gtest/gtest.h"

#include "vsf.h"

using namespace std;

class VsfTest : public ::testing::Test {
protected:
    VsfTest() {
        vsf = Vsf::get_instance();
    }

    Vsf* vsf;
    set<void*> ff;
};

void print_callback(
    const VmId& vm_id,
    const string& name,
    const string& uuid,
    const int vsocket_num,
    const int vcore_num,
    const int vhpthread_num,
    const int total_mem_size,
    const set<pid_t>& vcpu_ids,
    const set<pid_t>& stable_vmthread_ids
    )
{
    cout << "[vm_base]print_callback:" << vm_id << ":"
        << name << ":"
        << uuid << ":"
        << vsocket_num << ":"
        << vcore_num << ":"
        << vhpthread_num << ":"
        << total_mem_size << ":"
        << "[";
    for (auto& id : vcpu_ids)
        cout << id << ":"; 
    cout << "]:[";
    for (auto& id : stable_vmthread_ids)
        cout << id << ":"; 
    cout << "]" << endl;
}

void print_callback2(
    const VmId& vm_id,
    const string& name,
    const string& uuid,
    const int vsocket_num,
    const int vcore_num,
    const int vhpthread_num,
    const int total_mem_size,
    const set<pid_t>& vcpu_ids,
    const set<pid_t>& stable_vmthread_ids
    )
{
    cout << "[vm_base]print_callback_222:" << vm_id << ":"
        << name << ":"
        << uuid << ":"
        << vsocket_num << ":"
        << vcore_num << ":"
        << vhpthread_num << ":"
        << total_mem_size << ":"
        << "[";
    for (auto& id : vcpu_ids)
        cout << id << ":"; 
    cout << "]:[";
    for (auto& id : stable_vmthread_ids)
        cout << id << ":"; 
    cout << "]" << endl;
}

TEST_F(VsfTest, vm_base)
{
    vsf->init({ 
        { Option::OP_VM_BASE,
            {
                { OptionParam::VM_CMD, "qemu-system-x86_64_AAA" },
                { OptionParam::LOOP_INTERVAL, 2000 },
                { OptionParam::CALLBACK, VmBaseCallback(print_callback) }
            }
        }
    });
    Host *host = vsf->init_host();

    set<VM> vms = vsf->init_vms(host);

    sleep(5);
    vsf->clear_param({
        Option::OP_VM_BASE 
    });
    vsf->set_param({
        { Option::OP_VM_BASE,
            {
                { OptionParam::VM_CMD, "qemu-system-x86_64" },
                { OptionParam::LOOP_INTERVAL, 2500 },
                { OptionParam::CALLBACK, VmBaseCallback(print_callback2) }
            }
        }
    });
    sleep(5);

    for (auto& vm : vms)
    {
        cout << vm.vm_id() << ":" << vm.name() << ":" << vm.uuid() << ":" << vm.vsocket_num() << ":" << vm.vcore_num() << ":" << vm.vhpthread_num() << ":" << vm.total_mem_size() << ":" << vm.vcpu_num() << ":" << vm.stable_vmthread_num() << ":" << vm.volatile_vmthread_num();
        set<pid_t> pid_set = vm.vcpu_ids();
        for (auto& pid : pid_set)
            cout << "|" << pid;
        cout << "^";
        pid_set = vm.stable_vmthread_ids();
        for (auto& pid : pid_set)
            cout << "|" << pid;
        cout << "^";
        pid_set = vm.volatile_vmthread_ids();
        for (auto& pid : pid_set)
            cout << "|" << pid;
        cout << endl;
    }
}

