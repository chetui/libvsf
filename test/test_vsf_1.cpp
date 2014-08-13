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
};

TEST_F(VsfTest, vm_base)
{
    vsf->init({ 
        { Option::OP_VM_BASE,
            {
                { OptionParam::VM_CMD, "qemu-system-x86_64" }
            }
        }
    });
    Host *host = vsf->init_host();

    set<VM> vms = vsf->init_vms(host);

    for (auto& vm : vms)
    {
        cout << vm.vm_id() << ":" << vm.name() << ":" << vm.uuid() << ":" << vm.vsocket_num() << ":" << vm.vcore_num() << ":" << vm.vhpthread_num() << ":" << vm.total_mem_size() << ":" << vm.vcpu_num() << ":" << vm.stable_vmthread_num();
        set<pid_t> pid_set = vm.vcpu_ids();
        for (auto& pid : pid_set)
            cout << "|" << pid;
        cout << "^";
        pid_set = vm.stable_vmthread_ids();
        for (auto& pid : pid_set)
            cout << "|" << pid;
        cout << endl;
    }
}

