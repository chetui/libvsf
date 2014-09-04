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

void print_callback(const CacheMissData& data)
{
    cout << "[vm_cache_miss]print_callback:" << data << endl;
}

TEST_F(VsfTest, vm_cache_miss)
{
    vsf->init({ 
        { Option::OP_VM_CACHE_MISS,
            {
                { OptionParam::LOOP_INTERVAL, 1000 },
                { OptionParam::SAMPLE_INTERVAL, 50000 },
                { OptionParam::CALLBACK, VmCacheMissCallback(print_callback) }
            }
        }
    });
    Host *host = vsf->init_host();

    set<VM> vms = vsf->init_vms(host);

    for (auto& vm : vms)
    {
        cout << vm.vm_id() << "'s cache_miss:" << vm.cache_miss() << endl;
        set<pid_t> pid_set = vm.stable_vmthread_ids();
        for (auto& pid : pid_set)
            cout << pid << ":" << vm.cache_miss(pid) << endl;
        pid_set = vm.volatile_vmthread_ids();
        for (auto& pid : pid_set)
            cout << pid << ":" << vm.cache_miss(pid) << endl;
    }
}

