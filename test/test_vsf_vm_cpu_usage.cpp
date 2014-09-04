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

void print_callback(pid_t pid, pid_t tid, int cpu_usage)
{
    std::cout << "[vm_cpu_usage]print_callback:" << pid << ":" << tid << ":" << cpu_usage << std::endl;
}

TEST_F(VsfTest, vm_cpu_usage)
{
    vsf->init({ 
        { Option::OP_VM_CPU_USAGE,
            {
                { OptionParam::LOOP_INTERVAL, 1000 },
                { OptionParam::CALLBACK, VmCpuUsageCallback(print_callback) }
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

