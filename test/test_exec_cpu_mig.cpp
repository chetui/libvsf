#include <cstdio>
#include <iostream>
#include <set>
#include <algorithm>
#include <iterator>
#include "gtest/gtest.h"

#include "sysinfo/vm/dynamic/vm_base.h"
#include "sysinfo/host/static/node_cpu.h"
#include "exec/cpu_mig.h"

using namespace std;

class CpuMigTest : public ::testing::Test {
protected:
    CpuMigTest() {
        vm_base = VmBase::get_instance();
        cpu_mig = CpuMig::get_instance();
    }

    VmBase* vm_base;
    CpuMig* cpu_mig;
};

TEST_F(CpuMigTest, cpu_mig)
{
    set<int> hpthread_ids = { 1, 3, 5 };
    set<int> all_ids = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    set<VmId> vm_ids = vm_base->get_vm_ids();
    map<VmId, set<pid_t> > pids;
    for (auto& vm_id : vm_ids) {
        set<pid_t> stable_vmthread_ids = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthread_ids = vm_base->get_volatile_vmthread_ids(vm_id);
        set<pid_t> u;
        set_union(
            stable_vmthread_ids.begin(), stable_vmthread_ids.end(),
            volatile_vmthread_ids.begin(), volatile_vmthread_ids.end(),
            inserter(u, u.end()));
        pids[vm_id] = u;
        for (auto& pid : u)
            cpu_mig->set_cpu_mig(pid, hpthread_ids);
    }

    cpu_mig->exec_cpu_mig();

    char buf[10240];
    for (auto& vm_id : vm_ids) {
        string cmd = "virsh vcpuinfo " + vm_base->get_name(vm_id) + "| grep 'Affinity'";
        FILE* data = popen(cmd.c_str(), "r");
        while (fgets(buf, sizeof(buf), data)) {
            cout << buf;
        }
        pclose(data);
        for (auto& pid : pids[vm_id])
            cpu_mig->set_cpu_mig(pid, all_ids);
    }

    cpu_mig->exec_cpu_mig();

    cout << "clean------------" << endl;

    for (auto& vm_id : vm_ids) {
        cout << "vm_id: " << vm_id << endl;
        string cmd = "virsh vcpuinfo " + vm_base->get_name(vm_id) + "| grep 'Affinity'";
        cout << "cmd: " << cmd << endl;
        FILE* data = popen(cmd.c_str(), "r");
        while (fgets(buf, sizeof(buf), data)) {
            cout << buf;
        }
        pclose(data);
    }

}
