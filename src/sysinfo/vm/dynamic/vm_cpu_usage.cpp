#include "sysinfo/vm/dynamic/vm_cpu_usage.h"

#include <mutex>
#include <shared_mutex>
#include <algorithm>
#include "utils/log.h"

using namespace std;

constexpr const int VmCpuUsage::DEFAULT_INTERVAL_MS;

VmCpuUsage::VmCpuUsage(): interval_ms_(DEFAULT_INTERVAL_MS), callback_func_(new cpu_usage_callback_t)
{
    *callback_func_ = nullptr;

    vm_base_ = VmBase::get_instance();
    cpu_usage_ = CpuUsage::get_instance();

    cpu_usage_->set_callback(cpu_usage_callback);
}

VmCpuUsage::~VmCpuUsage()
{
    stop();
}

VmCpuUsage *VmCpuUsage::get_instance()
{
    static VmCpuUsage vm_cpu_usage;
    return &vm_cpu_usage;
}

void VmCpuUsage::set_interval(int interval_ms) 
{
    interval_ms_ = interval_ms;
}

void VmCpuUsage::set_callback(cpu_usage_callback_t callback_func) 
{
    *callback_func_ = callback_func;
}

void VmCpuUsage::clear_param()
{
    cpu_usage_->clear_param();
    cpu_usage_->set_callback(cpu_usage_callback);
    interval_ms_ = DEFAULT_INTERVAL_MS;
    unique_lock<shared_timed_mutex> lock(cpu_usage_->get_data_mutex());
    *callback_func_ = nullptr;;
}

int VmCpuUsage::get_sys_cpu_usage()
{
    refresh_twice();
    return cpu_usage_->get_sys_cpu_usage_without_refresh();
}

int VmCpuUsage::get_cpu_usage(VmId vm_id)
{
    refresh_twice();
    return cpu_usage_->get_process_cpu_usage_without_refresh(vm_id.pid);
}

int VmCpuUsage::get_cpu_usage(pid_t vmthread_id)
{
    refresh_twice();
    return cpu_usage_->get_thread_cpu_usage_without_refresh(vmthread_id);
}

HpthreadId VmCpuUsage::get_running_on_hpthread(pid_t vmthread_id)
{
    refresh_twice();
    return HpthreadId(cpu_usage_->get_thread_running_on_cpu_without_refresh(vmthread_id));
}

void VmCpuUsage::cpu_usage_callback(pid_t pid, pid_t tid, int cpu_usage)
{
    VmCpuUsage* vm_cpu_usage = VmCpuUsage::get_instance();
    if (*(vm_cpu_usage->callback_func_)) {
        (*(vm_cpu_usage->callback_func_))(pid, tid, cpu_usage);
    }
}

void VmCpuUsage::refresh_twice()
{
    if (!joinable()) {
        refresh();
        this_thread::sleep_for(chrono::milliseconds(interval_ms_));
        refresh();
    }
}

void VmCpuUsage::refresh()
{
    shared_timed_mutex& lock(cpu_usage_->get_data_mutex());
    lock.lock();

    set<VmId> vm_ids = vm_base_->get_vm_ids();
    set<pid_t> pids;
    for (auto& vm_id : vm_ids)
        pids.insert(vm_id.pid);

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
        cpu_usage_->start_watching(pid);
    for (auto& pid : to_stop)
        cpu_usage_->stop_watching(pid);

    lock.unlock();

    cpu_usage_->refresh();

//    LDEBUG << "DDDD: interval_ms_: " << interval_ms_ << endl;
    return;
}

void VmCpuUsage::clear()
{
    unique_lock<shared_timed_mutex> lock(cpu_usage_->get_data_mutex());
    cpu_usage_->clear();
    last_pids_.clear();
}

void VmCpuUsage::run()
{
    while(!stop_)
    {
        refresh();
        this_thread::sleep_for(chrono::milliseconds(interval_ms_));
    }
    clear();
}
