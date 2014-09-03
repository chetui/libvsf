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
    callback_func_ = new cache_miss_callback_t;
    *callback_func_ = nullptr;

    vm_base_ = VmBase::get_instance();
    cache_miss_ = CacheMiss::get_instance();

    cache_miss_->set_callback(cal_vm_miss_rate);
}

VmCacheMiss::~VmCacheMiss()
{
    stop();
    delete callback_func_;
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

void VmCacheMiss::set_callback(cache_miss_callback_t callback_func) 
{
    *callback_func_ = callback_func;
}

CacheMissData VmCacheMiss::get_cache_miss(VmId vm_id)
{
    if (!joinable())
    {
        refresh();
    }
    while (!(cache_miss_->get_has_data()))
        this_thread::sleep_for(chrono::milliseconds(10));

    shared_lock<shared_timed_mutex> lock(cache_miss_->get_data_mutex());
    auto iter = vm_cache_miss_data_.find(vm_id);
    if (iter != vm_cache_miss_data_.end())
        return iter->second;
    else
        return {vm_id.pid};
}

CacheMissData VmCacheMiss::get_cache_miss(pid_t vmthread_id)
{
    if (!joinable())
        refresh();

    return cache_miss_->get_cache_miss_without_refresh(vmthread_id);
}

void VmCacheMiss::cal_vm_miss_rate(const CacheMissData& data)
{
    VmCacheMiss* vm_cache_miss = VmCacheMiss::get_instance(); 
    VmId vm_id = VmBase::get_instance()->stable_vmthread_id_to_vm_id(data.tid);
    if (vm_id.start_timestamp == 0) { //not found
        vm_id = vm_cache_miss->volatile_vmthread_id_to_vm_id_[data.tid];
        if (vm_id.start_timestamp == 0) {//not found again
            LDEBUG << "not found vm_id of CacheMissData::tid " << data.tid;
            return;
        }
    }
    CacheMissData& cache_miss_data = VmCacheMiss::get_instance()->vm_cache_miss_data_[vm_id];
    cache_miss_data.cache_misses.count += data.cache_misses.count;
    cache_miss_data.cpu_cycles.count += data.cpu_cycles.count;
    cache_miss_data.instructions.count += data.instructions.count;
    cache_miss_data.cache_references.count += data.cache_references.count;
    cache_miss_data.update_miss_rate();

    if (*(vm_cache_miss->callback_func_)) {
        (*(vm_cache_miss->callback_func_))(data);
    }

//    LDEBUG << "callback:" 
//        << vm_id << ":"
//        << data.tid << ":"
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
    set<pid_t> tids;
    for (auto& vm_id : vm_ids) {
        set<pid_t> stable_vmthread_ids = vm_base_->get_stable_vmthread_ids(vm_id);
        set<pid_t> volatile_vmthread_ids = vm_base_->get_volatile_vmthread_ids(vm_id);
        tids.insert(stable_vmthread_ids.begin(), stable_vmthread_ids.end());
        tids.insert(volatile_vmthread_ids.begin(), volatile_vmthread_ids.end());
        for (auto& tid : volatile_vmthread_ids)
            volatile_vmthread_id_to_vm_id_[tid] = vm_id;
    }

    vector<pid_t> to_start;
    vector<pid_t> to_stop;
    set_difference(
        tids.begin(), tids.end(),
        last_tids_.begin(), last_tids_.end(),
        back_inserter(to_start));
    set_difference(
        last_tids_.begin(), last_tids_.end(),
        tids.begin(), tids.end(),
        back_inserter(to_stop));
    last_tids_ = tids;
    for (auto& tid : to_start)
        cache_miss_->start_watching(tid);
    for (auto& tid : to_stop)
        cache_miss_->stop_watching(tid);

    lock.unlock();

    cache_miss_->refresh();
}

void VmCacheMiss::clear()
{
    unique_lock<shared_timed_mutex> lock(cache_miss_->get_data_mutex());

    cache_miss_->clear();

    vm_cache_miss_data_.clear();
    last_tids_.clear();
    volatile_vmthread_id_to_vm_id_.clear();
}

void VmCacheMiss::run()
{
    while(!stop_) {
        refresh();
        this_thread::sleep_for(chrono::milliseconds(loop_interval_ms_));
    }
    clear();
}
