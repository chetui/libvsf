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

TEST_F(VsfTest, cpu_mig)
{
    vsf->init({ 
    });
    Host *host = vsf->init_host();

    set<VM> vms = vsf->init_vms(host);

    set<HpthreadId> part_ids = { 1, 3, 5 };
    set<HpthreadId> all_ids = host->hpthread_ids();

    char buf[10240];
    for (auto& vm : vms) {
        cout << vm.vm_id() << endl;
        set<pid_t> stable_pid_set = vm.stable_vmthread_ids();
        for (auto& pid : stable_pid_set)
            vm.set_vcpu_mig(pid, part_ids);
        set<pid_t> volatile_pid_set = vm.volatile_vmthread_ids();
        for (auto& pid : volatile_pid_set)
            vm.set_vcpu_mig(pid, part_ids);

        vsf->exec_mig();

        string cmd = "virsh vcpuinfo " + vm.name() + "| grep 'Affinity'";
        FILE* data = popen(cmd.c_str(), "r");
        while (fgets(buf, sizeof(buf), data)) {
            cout << buf;
        }
        pclose(data);

        for (auto& pid : stable_pid_set)
            vm.set_vcpu_mig(pid, all_ids);
        for (auto& pid : volatile_pid_set)
            vm.set_vcpu_mig(pid, all_ids);

        vsf->exec_mig();
        cout << "clean------------" << endl;

        data = popen(cmd.c_str(), "r");
        while (fgets(buf, sizeof(buf), data)) {
            cout << buf;
        }
        pclose(data);
    }
}

