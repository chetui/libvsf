#include <unistd.h>
#include <iostream>
#include <set>
#include "gtest/gtest.h"

#include "sysinfo/vm/dynamic/vm_base.h"
#include "sysinfo/vm/dynamic/vm_cache_miss.h"

using namespace std;

class VmCacheMissTest : public ::testing::Test {
protected:
    VmCacheMissTest() {
        vm_base = VmBase::get_instance();
        vm_cache_miss = VmCacheMiss::get_instance();
    }

    VmCacheMiss* vm_cache_miss;
    VmBase* vm_base;
};

TEST_F(VmCacheMissTest, vm_cache_miss_with_thread_with_vm_base)
{
    vm_base->start();
    cout << "TTT" << endl;
    vm_cache_miss->start();
    cout << "TTT1" << endl;
    set<VmId> vm_ids = vm_base->get_vm_ids();
    for(auto& vm_id : vm_ids) {
        cout << "TTT2" << endl;
        cout << vm_id << "'s cache_miss:" << vm_cache_miss->get_cache_miss(vm_id) << endl;
        set<pid_t> stable_vmthreads = vm_base->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthreads = vm_base->get_volatile_vmthread_ids(vm_id);
        for (auto& v : stable_vmthreads) {
            cout << v << ":" << vm_cache_miss->get_cache_miss(v) << endl;
        }
        for (auto& v : volatile_vmthreads) {
            cout << v << ":" << vm_cache_miss->get_cache_miss(v) << endl;
        }
    }
    cout << "TTT3" << endl;
    vm_cache_miss->stop();
    cout << "TTT4" << endl;
    vm_base->stop();
    cout << "TTT5" << endl;
}

