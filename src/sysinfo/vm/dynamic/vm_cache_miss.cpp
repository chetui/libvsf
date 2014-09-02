#include "sysinfo/vm/dynamic/vm_cache_miss.h"

#include <mutex>
#include <shared_mutex>
#include <map>
#include <vector>
#include <algorithm>
#include "utils/log.h"

using namespace std;

VmCacheMiss::VmCacheMiss():
    loop_interval_ms_(1000)
{
    vm_base_ = VmBase::get_instance();
    cache_miss_ = CacheMiss::get_instance();

    cache_miss_->set_callback(cal_vm_miss_rate);
}

VmCacheMiss::~VmCacheMiss()
{
    stop();
}

VmCacheMiss* VmCacheMiss::get_instance()
{
    static VmCacheMiss vm_cache_miss;
    return &vm_cache_miss;
}

void VmCacheMiss::set_loop_interval(int interval_ms)
{
    loop_interval_ms_ = interval_ms;
}

void VmCacheMiss::set_sample_interval(int interval_us) 
{
    cache_miss_->set_sample_interval(interval_us);
}

CacheMissData VmCacheMiss::get_cache_miss(VmId vm_id)
{
    if (!joinable())
        refresh();
    while (!(cache_miss_->get_has_data()))
        this_thread::sleep_for(chrono::milliseconds(10));

    shared_lock<shared_timed_mutex> lock(cache_miss_->get_data_mutex());
    auto iter = vm_cache_miss_data_.find(vm_id);
    if (iter != vm_cache_miss_data_.end())
        return iter->second;
    else
        return {vm_id.pid};
}

CacheMissData VmCacheMiss::get_cache_miss(pid_t vmthread)
{
    if (!joinable())
        refresh();

    return cache_miss_->get_cache_miss_without_refresh(vmthread);
}

void VmCacheMiss::cal_vm_miss_rate(const CacheMissData& data)
{
    VmId vm_id = VmBase::get_instance()->stable_vmthread_id_to_vm_id(data.pid);
    if (vm_id.start_timestamp == 0) { //not found
        vm_id = VmCacheMiss::get_instance()->volatile_vmthread_id_to_vm_id_[data.pid];
        if (vm_id.start_timestamp == 0) {//not found again
            LDEBUG << "not found vm_id of CacheMissData::pid " << data.pid;
            return;
        }
    }
    CacheMissData& cache_miss_data = VmCacheMiss::get_instance()->vm_cache_miss_data_[vm_id];
    cache_miss_data.cache_misses.count += data.cache_misses.count;
    cache_miss_data.cpu_cycles.count += data.cpu_cycles.count;
    cache_miss_data.instructions.count += data.instructions.count;
    cache_miss_data.cache_references.count += data.cache_references.count;
    cache_miss_data.update_miss_rate();
//    LDEBUG << "callback:" 
//        << vm_id << ":"
//        << data.pid << ":"
//        << data.cache_misses.count << ":"
//        << data.cpu_cycles.count << ":"
//        << data.instructions.count << ":"
//        << data.cache_references.count;
}

void VmCacheMiss::refresh()
{
    shared_timed_mutex& lock(cache_miss_->get_data_mutex());
    lock.lock();
    vm_cache_miss_data_.clear();

    set<VmId> vm_ids = vm_base_->get_vm_ids();
    set<pid_t> pids;
    for (auto& vm_id : vm_ids) {
        set<pid_t> stable_vmthread_ids = vm_base_->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthread_ids = vm_base_->get_volatile_vmthread_ids(vm_id);
        pids.insert(stable_vmthread_ids.begin(), stable_vmthread_ids.end());
        pids.insert(volatile_vmthread_ids.begin(), volatile_vmthread_ids.end());
        for (auto& pid : volatile_vmthread_ids)
            volatile_vmthread_id_to_vm_id_[pid] = vm_id;
    }

    vector<pid_t> to_start;
    vector<pid_t> to_stop;
    set_difference(
        pids.begin(), pids.end(),
        last_pids_.begin(), last_pids_.end(),
        back_inserter(to_start));
    set_difference(
        last_pids_.begin(), last_pids_.end(),
        pids.begin(), pids.end(),
        back_inserter(to_stop));
    last_pids_ = pids;
    for (auto& pid : to_start)
        cache_miss_->start_watching(pid);
    for (auto& pid : to_stop)
        cache_miss_->stop_watching(pid);

    lock.unlock();

    cache_miss_->refresh();
}

void VmCacheMiss::clear()
{
    unique_lock<shared_timed_mutex> lock(cache_miss_->get_data_mutex());

    vm_cache_miss_data_.clear();
    last_pids_.clear();
    volatile_vmthread_id_to_vm_id_.clear();

    cache_miss_->clear();
}

void VmCacheMiss::run()
{
    while(!stop_) {
        refresh();
        this_thread::sleep_for(chrono::milliseconds(loop_interval_ms_));
    }
    clear();
}
