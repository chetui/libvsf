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

void print_callback(const CacheMissData& data)
{
    std::cout << "print_callback:" << data << std::endl;
}

void print_callback2(const CacheMissData& data)
{
    std::cout << "print_callback_222:" << data << std::endl;
}

TEST_F(CacheMissTest, cache_miss_with_thread_with_vm_base_with_clear_param)
{
    vm_base->start();
    cache_miss->set_loop_interval(2000);
    cache_miss->set_sample_interval(80000);
    cache_miss->set_callback(print_callback);
    cache_miss->start();

    shared_timed_mutex& lock(cache_miss->get_data_mutex());
    lock.lock();

    set<VmId> vm_ids = vm_base->get_vm_ids();
    set<pid_t> pids;
    for (auto& vm_id : vm_ids) {
        set<pid_t> stable_vmthread_ids = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthread_ids = vm_base->get_volatile_vmthread_ids(vm_id);
        pids.insert(stable_vmthread_ids.begin(), stable_vmthread_ids.end());
        pids.insert(volatile_vmthread_ids.begin(), volatile_vmthread_ids.end());
    }

    for (auto& pid : pids)
        cache_miss->start_watching(pid);

    lock.unlock();

    sleep(4);
    cache_miss->clear_param();
    cache_miss->set_loop_interval(1700);
    cache_miss->set_sample_interval(75000);
    cache_miss->set_callback(print_callback2);
    sleep(4);
    for (auto& pid : pids) {
        cout << pid << ":" << cache_miss->get_cache_miss(pid) << endl;
    }

    cache_miss->stop();
    vm_base->stop();
}

TEST_F(CacheMissTest, cache_miss_without_thread_with_vm_base)
{
    vm_base->start();
    cache_miss->set_loop_interval(2000);
    cache_miss->set_sample_interval(80000);
    cache_miss->set_callback(nullptr);

    shared_timed_mutex& lock(cache_miss->get_data_mutex());
    lock.lock();

    set<VmId> vm_ids = vm_base->get_vm_ids();
    set<pid_t> pids;
    for (auto& vm_id : vm_ids) {
        set<pid_t> stable_vmthread_ids = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthread_ids = vm_base->get_volatile_vmthread_ids(vm_id);
        pids.insert(stable_vmthread_ids.begin(), stable_vmthread_ids.end());
        pids.insert(volatile_vmthread_ids.begin(), volatile_vmthread_ids.end());
    }

    for (auto& pid : pids)
        cache_miss->start_watching(pid);

    lock.unlock();

    cache_miss->refresh();

    for (auto& pid : pids) {
        cout << pid << ":" << cache_miss->get_cache_miss_without_refresh(pid) << endl;
    }

    lock.lock();
    cache_miss->clear();
    lock.unlock();
    vm_base->stop();
}

TEST_F(CacheMissTest, cache_miss_with_thread_without_vm_base)
{
    cache_miss->set_loop_interval(2000);
    cache_miss->set_sample_interval(80000);
    cache_miss->start();

    shared_timed_mutex& lock(cache_miss->get_data_mutex());
    lock.lock();

    set<VmId> vm_ids = vm_base->get_vm_ids();
    set<pid_t> pids;
    for (auto& vm_id : vm_ids) {
        set<pid_t> stable_vmthread_ids = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthread_ids = vm_base->get_volatile_vmthread_ids(vm_id);
        pids.insert(stable_vmthread_ids.begin(), stable_vmthread_ids.end());
        pids.insert(volatile_vmthread_ids.begin(), volatile_vmthread_ids.end());
    }

    for (auto& pid : pids)
        cache_miss->start_watching(pid);

    lock.unlock();

    for (auto& pid : pids) {
        cout << pid << ":" << cache_miss->get_cache_miss(pid) << endl;
    }

    cache_miss->stop();
}

TEST_F(CacheMissTest, cache_miss_without_thread_without_vm_base)
{
    cache_miss->set_loop_interval(2000);
    cache_miss->set_sample_interval(80000);

    shared_timed_mutex& lock(cache_miss->get_data_mutex());
    lock.lock();

    set<VmId> vm_ids = vm_base->get_vm_ids();
    set<pid_t> pids;
    for (auto& vm_id : vm_ids) {
        set<pid_t> stable_vmthread_ids = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthread_ids = vm_base->get_volatile_vmthread_ids(vm_id);
        pids.insert(stable_vmthread_ids.begin(), stable_vmthread_ids.end());
        pids.insert(volatile_vmthread_ids.begin(), volatile_vmthread_ids.end());
    }

    for (auto& pid : pids)
        cache_miss->start_watching(pid);

    lock.unlock();

    cache_miss->refresh();

    for (auto& pid : pids) {
        cout << pid << ":" << cache_miss->get_cache_miss_without_refresh(pid) << endl;
    }

    lock.lock();
    cache_miss->clear();
    lock.unlock();
}

