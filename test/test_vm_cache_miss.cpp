#include <unistd.h>
#include <iostream>
#include <set>
#include "gtest/gtest.h"

#include "sysinfo/vm/dynamic/vm_base.h"
#include "sysinfo/vm/dynamic/cache_miss.h"

using namespace std;

class CacheMissTest : public ::testing::Test {
protected:
    CacheMissTest() {
        vm_base = VmBase::get_instance();
        cache_miss = CacheMiss::get_instance();
    }

    CacheMiss* cache_miss;
    VmBase* vm_base;
};

TEST_F(CacheMissTest, sys_cpu_usage_without_thread)
{
    cout << "system_cpu_usage:" << cache_miss->get_sys_cpu_usage() << endl;
    sleep(2);
    cout << "system_cpu_usage:" << cache_miss->get_sys_cpu_usage() << endl;
}

TEST_F(CacheMissTest, sys_cpu_usage_with_thread)
{
    cache_miss->start();
    cout << "system_cpu_usage:" << cache_miss->get_sys_cpu_usage() << endl;
    sleep(2);
    cout << "system_cpu_usage:" << cache_miss->get_sys_cpu_usage() << endl;
    cache_miss->stop();
}

TEST_F(CacheMissTest, cache_miss_without_thread_with_vm_base)
{
    vm_base->start();
    set<VmId> vm_ids = vm_base->get_vm_ids();
    cache_miss->get_sys_cpu_usage();
    sleep(2);
    for(auto& vm_id : vm_ids) {
        cout << vm_id << "'s cpu_usage:" << cache_miss->get_cpu_usage(vm_id) << endl;
        sleep(1);
        set<pid_t> stable_vmthreads = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthreads = vm_base->get_volatile_vmthread_ids(vm_id);
        for (auto& v : stable_vmthreads) {
            cout << v << ":" << cache_miss->get_cpu_usage(v); 
            sleep(1);
            cout << "[ON]" << cache_miss->get_running_on_hpthread(v) << endl;
            sleep(1);
        }
        for (auto& v : volatile_vmthreads) {
            cout << v << ":" << cache_miss->get_cpu_usage(v);
            sleep(1);
            cout << "[ON]" << cache_miss->get_running_on_hpthread(v) << endl;
            sleep(1);
        }
    }
    vm_base->stop();
}

TEST_F(CacheMissTest, cache_miss_without_thread_without_vm_base)
{
    set<VmId> vm_ids = vm_base->get_vm_ids();
    cache_miss->get_sys_cpu_usage();
    sleep(2);
    for(auto& vm_id : vm_ids) {
        cout << vm_id << "'s cpu_usage:" << cache_miss->get_cpu_usage(vm_id) << endl;
        sleep(1);
        set<pid_t> stable_vmthreads = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthreads = vm_base->get_volatile_vmthread_ids(vm_id);
        for (auto& v : stable_vmthreads) {
            cout << v << ":" << cache_miss->get_cpu_usage(v); 
            sleep(1);
            cout << "[ON]" << cache_miss->get_running_on_hpthread(v) << endl;
            sleep(1);
        }
        for (auto& v : volatile_vmthreads) {
            cout << v << ":" << cache_miss->get_cpu_usage(v);
            sleep(1);
            cout << "[ON]" << cache_miss->get_running_on_hpthread(v) << endl;
            sleep(1);
        }
    }
}


TEST_F(CacheMissTest, cache_miss_with_thread_with_vm_base)
{
    vm_base->start();
    cache_miss->start();
    set<VmId> vm_ids = vm_base->get_vm_ids();
    sleep(2);
    for(auto& vm_id : vm_ids) {
        cout << vm_id << "'s cpu_usage:" << cache_miss->get_cpu_usage(vm_id) << endl;
        set<pid_t> stable_vmthreads = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthreads = vm_base->get_volatile_vmthread_ids(vm_id);
        for (auto& v : stable_vmthreads) {
            cout << v << ":" << cache_miss->get_cpu_usage(v) << "[ON]" << cache_miss->get_running_on_hpthread(v) << endl;
        }
        for (auto& v : volatile_vmthreads) {
            cout << v << ":" << cache_miss->get_cpu_usage(v) << "[ON]" << cache_miss->get_running_on_hpthread(v) << endl;
        }
    }
    cache_miss->stop();
    vm_base->stop();
}

TEST_F(CacheMissTest, cache_miss_with_thread_without_vm_base)
{
    cache_miss->start();
    set<VmId> vm_ids = vm_base->get_vm_ids();
    sleep(2);
    for(auto& vm_id : vm_ids) {
        cout << vm_id << "'s cpu_usage:" << cache_miss->get_cpu_usage(vm_id) << endl;
        set<pid_t> stable_vmthreads = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthreads = vm_base->get_volatile_vmthread_ids(vm_id);
        for (auto& v : stable_vmthreads) {
            cout << v << ":" << cache_miss->get_cpu_usage(v) << "[ON]" << cache_miss->get_running_on_hpthread(v) << endl;
        }
        for (auto& v : volatile_vmthreads) {
            cout << v << ":" << cache_miss->get_cpu_usage(v) << "[ON]" << cache_miss->get_running_on_hpthread(v) << endl;
        }
    }
    cache_miss->stop();
}

