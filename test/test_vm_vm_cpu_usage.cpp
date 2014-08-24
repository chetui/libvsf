#include <unistd.h>
#include <iostream>
#include <set>
#include "gtest/gtest.h"

#include "sysinfo/vm/dynamic/vm_base.h"
#include "sysinfo/vm/dynamic/vm_cpu_usage.h"

using namespace std;

class VmCpuUsageTest : public ::testing::Test {
protected:
    VmCpuUsageTest() {
        vm_base = VmBase::get_instance();
        vm_cpu_usage = VmCpuUsage::get_instance();
    }

    VmCpuUsage* vm_cpu_usage;
    VmBase* vm_base;
};

TEST_F(VmCpuUsageTest, sys_cpu_usage_without_thread)
{
    cout << "system_cpu_usage:" << vm_cpu_usage->get_sys_cpu_usage() << endl;
    sleep(2);
    cout << "system_cpu_usage:" << vm_cpu_usage->get_sys_cpu_usage() << endl;
}

TEST_F(VmCpuUsageTest, sys_cpu_usage_with_thread)
{
    vm_cpu_usage->start();
    cout << "system_cpu_usage:" << vm_cpu_usage->get_sys_cpu_usage() << endl;
    sleep(2);
    cout << "system_cpu_usage:" << vm_cpu_usage->get_sys_cpu_usage() << endl;
    vm_cpu_usage->stop();
}

TEST_F(VmCpuUsageTest, vm_cpu_usage_without_thread_with_vm_base)
{
    vm_base->start();
    set<VmId> vm_ids = vm_base->get_vm_ids();
    vm_cpu_usage->get_sys_cpu_usage();
    sleep(2);
    for(auto& vm_id : vm_ids) {
        cout << vm_id << "'s cpu_usage:" << vm_cpu_usage->get_cpu_usage(vm_id) << endl;
        sleep(1);
        set<pid_t> stable_vmthreads = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthreads = vm_base->get_volatile_vmthread_ids(vm_id);
        for (auto& v : stable_vmthreads) {
            cout << v << ":" << vm_cpu_usage->get_cpu_usage(v); 
            sleep(1);
            cout << "[ON]" << vm_cpu_usage->get_running_on_hpthread(v) << endl;
            sleep(1);
        }
        for (auto& v : volatile_vmthreads) {
            cout << v << ":" << vm_cpu_usage->get_cpu_usage(v);
            sleep(1);
            cout << "[ON]" << vm_cpu_usage->get_running_on_hpthread(v) << endl;
            sleep(1);
        }
    }
    vm_base->stop();
}

TEST_F(VmCpuUsageTest, vm_cpu_usage_without_thread_without_vm_base)
{
    set<VmId> vm_ids = vm_base->get_vm_ids();
    vm_cpu_usage->get_sys_cpu_usage();
    sleep(2);
    for(auto& vm_id : vm_ids) {
        cout << vm_id << "'s cpu_usage:" << vm_cpu_usage->get_cpu_usage(vm_id) << endl;
        sleep(1);
        set<pid_t> stable_vmthreads = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthreads = vm_base->get_volatile_vmthread_ids(vm_id);
        for (auto& v : stable_vmthreads) {
            cout << v << ":" << vm_cpu_usage->get_cpu_usage(v); 
            sleep(1);
            cout << "[ON]" << vm_cpu_usage->get_running_on_hpthread(v) << endl;
            sleep(1);
        }
        for (auto& v : volatile_vmthreads) {
            cout << v << ":" << vm_cpu_usage->get_cpu_usage(v);
            sleep(1);
            cout << "[ON]" << vm_cpu_usage->get_running_on_hpthread(v) << endl;
            sleep(1);
        }
    }
}


TEST_F(VmCpuUsageTest, vm_cpu_usage_with_thread_with_vm_base)
{
    vm_base->start();
    vm_cpu_usage->start();
    set<VmId> vm_ids = vm_base->get_vm_ids();
    sleep(2);
    for(auto& vm_id : vm_ids) {
        cout << vm_id << "'s cpu_usage:" << vm_cpu_usage->get_cpu_usage(vm_id) << endl;
        set<pid_t> stable_vmthreads = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthreads = vm_base->get_volatile_vmthread_ids(vm_id);
        for (auto& v : stable_vmthreads) {
            cout << v << ":" << vm_cpu_usage->get_cpu_usage(v) << "[ON]" << vm_cpu_usage->get_running_on_hpthread(v) << endl;
        }
        for (auto& v : volatile_vmthreads) {
            cout << v << ":" << vm_cpu_usage->get_cpu_usage(v) << "[ON]" << vm_cpu_usage->get_running_on_hpthread(v) << endl;
        }
    }
    vm_cpu_usage->stop();
    vm_base->stop();
}

TEST_F(VmCpuUsageTest, vm_cpu_usage_with_thread_without_vm_base)
{
    vm_cpu_usage->start();
    set<VmId> vm_ids = vm_base->get_vm_ids();
    sleep(2);
    for(auto& vm_id : vm_ids) {
        cout << vm_id << "'s cpu_usage:" << vm_cpu_usage->get_cpu_usage(vm_id) << endl;
        set<pid_t> stable_vmthreads = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthreads = vm_base->get_volatile_vmthread_ids(vm_id);
        for (auto& v : stable_vmthreads) {
            cout << v << ":" << vm_cpu_usage->get_cpu_usage(v) << "[ON]" << vm_cpu_usage->get_running_on_hpthread(v) << endl;
        }
        for (auto& v : volatile_vmthreads) {
            cout << v << ":" << vm_cpu_usage->get_cpu_usage(v) << "[ON]" << vm_cpu_usage->get_running_on_hpthread(v) << endl;
        }
    }
    vm_cpu_usage->stop();
}

