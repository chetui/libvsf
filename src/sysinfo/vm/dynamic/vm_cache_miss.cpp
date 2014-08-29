#include "sysinfo/vm/dynamic/vm_cache_miss.h"

#include <mutex>
#include <shared_mutex>

using namespace std;

VmCacheMiss::VmCacheMiss()
{
    vm_base = VmBase::get_instance();
    cache_miss = CacheMiss::get_instance();

    cache_miss->set_callback(cal_vm_miss_rate);
}

VmCacheMiss* VmCacheMiss::get_instance()
{
    static VmCacheMiss vm_cache_miss;
    return &vm_cache_miss;
}

void VmCacheMiss::set_interval(int interval_ms)
{
    loop_interval_ms = interval_ms;
}

CacheMissData VmCacheMiss::get_cache_miss(Vmid vm_id)
{
    if (!joinable())
        cache_miss->refresh();
    while (!(cache_miss->get_has_data()))
        this_thread::sleep_for(chrono::milliseconds(10));

    shared_lock<shared_timed_mutex> lock(cache_miss->get_data_mutex());
    auto iter = vm_cache_miss_data_.find(vm_id);
    if (iter != vm_cache_miss_data_.end())
        return iter-second;
    else
        return {vm_id.pid};
}

CacheMissData VmCacheMiss::get_cache_miss(Vmid vm_id, pid_t vmthread)
{
    vm_id = vm_id; //not used currently
    return cache_miss->get_cache_miss(vmthread);
}

void VmCacheMiss::cal_vm_miss_rate(const CacheMissData& data)
{
    VmId vm_id = vm_base->pid_to_vm_id(data.pid);
    CacheMissData& cache_miss_data = VmCacheMiss::get_instance()->vm_cache_miss_data_[vm_id];
    cache_miss_data.cache_misses.count += data.cache_misses.count;
    cache_miss_data.cpu_cycles.count += data.cpu_cycles.count;
    cache_miss_data.instructions.count += data.instructions.count;
    cache_miss_data.cache_references.count += data.cache_references.count;
    cache_miss_data.update_miss_rate();
}

void VmCacheMiss::clear()
{
    unique_lock<shared_timed_mutex> lock(cache_miss->get_data_mutex());
    vm_cache_miss_data_.clear();
}

void VmCacheMiss::run()
{
    while(!stop_) {
    }
    clear();
}
