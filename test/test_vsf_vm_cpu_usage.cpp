#include <unistd.h>
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

TEST_F(VsfTest, vm_cpu_usage)
{
    vsf->init({ 
        { Option::OP_VM_CPU_USAGE,
            {
                { OptionParam::INTERVAL, 1000 }
            }
        }
    });
    Host *host = vsf->init_host();

    set<VM> vms = vsf->init_vms(host);

    sleep(2);
    for (auto& vm : vms)
    {
        cout << vm.vm_id() << ":" << vm.sys_cpu_usage() << ":" << vm.cpu_usage() << endl;
        set<pid_t> pid_set = vm.stable_vmthread_ids();
        for (auto& pid : pid_set)
            cout << pid << ":" << vm.cpu_usage(pid) << "[ON]" << vm.running_on_hpthread(pid) << endl;
        pid_set = vm.volatile_vmthread_ids();
        for (auto& pid : pid_set)
            cout << pid << ":" << vm.cpu_usage(pid) << "[ON]" << vm.running_on_hpthread(pid) << endl;
    }
}

