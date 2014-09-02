#include <unistd.h>
#include <iostream>
#include <set>
#include "gtest/gtest.h"

#include "sysinfo/vm/dynamic/vm_base.h"
#include "sysinfo/vm/dynamic/cpu_usage.h"

using namespace std;

class CpuUsageTest : public ::testing::Test {
protected:
    CpuUsageTest() {
        vm_base = VmBase::get_instance();
        cpu_usage = CpuUsage::get_instance();
    }

    CpuUsage* cpu_usage;
    VmBase* vm_base;
};

void print_callback(const ThreadCpuUsageData& data)
{
    std::cout << "print_callback:" << data << std::endl;
}

TEST_F(CpuUsageTest, cpu_usage_with_thread_with_vm_base)
{
    vm_base->start();
    cpu_usage->set_interval(2000);
    cpu_usage->set_callback(print_callback);
    cpu_usage->start();

    shared_timed_mutex& lock(cpu_usage->get_data_mutex());
    lock.lock();

    set<VmId> vm_ids = vm_base->get_vm_ids();
    for (auto& vm_id : vm_ids) {
        cpu_usage->start_watching(vm_id.pid);
    }

    lock.unlock();

    cout << "sys " << cpu_usage->get_sys_cpu_usage() << endl;
    for (auto& vm_id : vm_ids) {
        cout << "process " << vm_id.pid << ":" << cpu_usage->get_process_cpu_usage(vm_id.pid) << endl;
        set<pid_t> stable_vmthread_ids = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthread_ids = vm_base->get_volatile_vmthread_ids(vm_id);
        for (auto pid : stable_vmthread_ids)
            cout << "stable thread " << pid << ":" << cpu_usage->get_thread_cpu_usage(pid) << "[ON]" << cpu_usage->get_thread_running_on_cpu(pid) << endl;
        for (auto pid : volatile_vmthread_ids)
            cout << "volatile thread " << pid << ":" << cpu_usage->get_thread_cpu_usage(pid) << "[ON]" << cpu_usage->get_thread_running_on_cpu(pid) << endl;
    }

    cpu_usage->stop();
    vm_base->stop();
}

TEST_F(CpuUsageTest, cpu_usage_without_thread_with_vm_base)
{
    vm_base->start();
    cpu_usage->set_interval(2000);
    cpu_usage->set_callback(print_callback);

    shared_timed_mutex& lock(cpu_usage->get_data_mutex());
    lock.lock();

    set<VmId> vm_ids = vm_base->get_vm_ids();
    set<pid_t> pids;
    for (auto& vm_id : vm_ids) {
        pids.insert(vm_id.pid);
    }

    for (auto& pid : pids)
        cpu_usage->start_watching(pid);

    lock.unlock();

    cpu_usage->refresh_twice();

    cout << "sys " << cpu_usage->get_sys_cpu_usage_without_refresh() << endl;
    for (auto& vm_id : vm_ids) {
        cout << "process " << vm_id.pid << ":" << cpu_usage->get_process_cpu_usage_without_refresh(vm_id.pid) << endl;
        set<pid_t> stable_vmthread_ids = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthread_ids = vm_base->get_volatile_vmthread_ids(vm_id);
        for (auto pid : stable_vmthread_ids)
            cout << "stable thread " << pid << ":" << cpu_usage->get_thread_cpu_usage_without_refresh(pid) << "[ON]" << cpu_usage->get_thread_running_on_cpu_without_refresh(pid) << endl;
        for (auto pid : volatile_vmthread_ids)
            cout << "volatile thread " << pid << ":" << cpu_usage->get_thread_cpu_usage_without_refresh(pid) << "[ON]" << cpu_usage->get_thread_running_on_cpu_without_refresh(pid) << endl;
    }

    lock.lock();
    cpu_usage->clear();
    lock.unlock();
    vm_base->stop();
}

TEST_F(CpuUsageTest, cpu_usage_with_thread_without_vm_base)
{
    cpu_usage->set_interval(2000);
    cpu_usage->set_callback(print_callback);
    cpu_usage->start();

    shared_timed_mutex& lock(cpu_usage->get_data_mutex());
    lock.lock();

    set<VmId> vm_ids = vm_base->get_vm_ids();
    set<pid_t> pids;
    for (auto& vm_id : vm_ids) {
        pids.insert(vm_id.pid);
    }

    for (auto& pid : pids)
        cpu_usage->start_watching(pid);

    lock.unlock();

    cout << "sys " << cpu_usage->get_sys_cpu_usage() << endl;
    for (auto& vm_id : vm_ids) {
        cout << "process " << vm_id.pid << ":" << cpu_usage->get_process_cpu_usage(vm_id.pid) << endl;
        set<pid_t> stable_vmthread_ids = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthread_ids = vm_base->get_volatile_vmthread_ids(vm_id);
        for (auto pid : stable_vmthread_ids)
            cout << "stable thread " << pid << ":" << cpu_usage->get_thread_cpu_usage(pid) << "[ON]" << cpu_usage->get_thread_running_on_cpu(pid) << endl;
        for (auto pid : volatile_vmthread_ids)
            cout << "volatile thread " << pid << ":" << cpu_usage->get_thread_cpu_usage(pid) << "[ON]" << cpu_usage->get_thread_running_on_cpu(pid) << endl;
    }

    cpu_usage->stop();
}

TEST_F(CpuUsageTest, cpu_usage_without_thread_without_vm_base)
{
    cpu_usage->set_interval(2000);
    cpu_usage->set_callback(print_callback);

    shared_timed_mutex& lock(cpu_usage->get_data_mutex());
    lock.lock();

    set<VmId> vm_ids = vm_base->get_vm_ids();
    set<pid_t> pids;
    for (auto& vm_id : vm_ids) {
        pids.insert(vm_id.pid);
    }

    for (auto& pid : pids)
        cpu_usage->start_watching(pid);

    lock.unlock();

    cpu_usage->refresh_twice();

    cout << "sys " << cpu_usage->get_sys_cpu_usage_without_refresh() << endl;
    for (auto& vm_id : vm_ids) {
        cout << "process " << vm_id.pid << ":" << cpu_usage->get_process_cpu_usage_without_refresh(vm_id.pid) << endl;
        set<pid_t> stable_vmthread_ids = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthread_ids = vm_base->get_volatile_vmthread_ids(vm_id);
        for (auto pid : stable_vmthread_ids)
            cout << "stable thread " << pid << ":" << cpu_usage->get_thread_cpu_usage_without_refresh(pid) << "[ON]" << cpu_usage->get_thread_running_on_cpu_without_refresh(pid) << endl;
        for (auto pid : volatile_vmthread_ids)
            cout << "volatile thread " << pid << ":" << cpu_usage->get_thread_cpu_usage_without_refresh(pid) << "[ON]" << cpu_usage->get_thread_running_on_cpu_without_refresh(pid) << endl;
    }

    lock.lock();
    cpu_usage->clear();
    lock.unlock();
}

