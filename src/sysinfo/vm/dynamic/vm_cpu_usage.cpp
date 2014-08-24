#include "sysinfo/vm/dynamic/vm_cpu_usage.h"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <functional>
#include "framework/exception.h"

using namespace std;

unsigned int VmCpuUsage::cpu_num_;

VmCpuUsage::VmCpuUsage(): has_data_(false)
{
    ms_interval_ = 1000;
    cpu_num_ = static_cast<unsigned int>(sysconf(_SC_NPROCESSORS_CONF));
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

void VmCpuUsage::set_interval(int ms_interval) 
{
    ms_interval_ = ms_interval;
}

int VmCpuUsage::get_sys_cpu_usage()
{
    refresh_and_wait();
    shared_lock<shared_timed_mutex> lock(data_mutex_);
    return static_cast<int>((total_cpu_time_old == 0 || total_cpu_time_delta == 0) ?
        0 : (1 - idle_time_delta * 1.0 / total_cpu_time_delta) * 100);
}

int VmCpuUsage::get_cpu_usage(VmId vm_id)
{
    refresh_and_wait();
    shared_lock<shared_timed_mutex> lock(data_mutex_);
    auto iter = vms_.find(vm_id);
    if (iter != vms_.end())
        return static_cast<int>(iter->second);
    else
        return -1;
}

int VmCpuUsage::get_cpu_usage(pid_t vmthread_id)
{
    refresh_and_wait();
    shared_lock<shared_timed_mutex> lock(data_mutex_);
    auto iter = vm_threads_.find(vmthread_id);
    if (iter != vm_threads_.end() && iter->second.valid_)
        return static_cast<int>(iter->second.cpu_usage_);
    else
        return -1;
}

HpthreadId VmCpuUsage::get_running_on_hpthread(pid_t vmthread_id)
{
    refresh_and_wait();
    shared_lock<shared_timed_mutex> lock(data_mutex_);
    auto iter = vm_threads_.find(vmthread_id);
    if (iter != vm_threads_.end())
        return HpthreadId(iter->second.processor_);
    else
        return HpthreadId();
}

void VmCpuUsage::refresh_and_wait()
{
    if(!joinable())
        refresh();
    while(!has_data_) 
        this_thread::sleep_for(chrono::milliseconds(10));
}

unsigned int VmCpuUsage::get_total_cpu_time_delta() const
{
    return (total_cpu_time_old == 0)?
        0: total_cpu_time_delta;
}

void VmCpuUsage::refresh()
{
    unique_lock<shared_timed_mutex> lock(data_mutex_);

    refresh_system();
    refresh_vm();

//    cout << "DDD----" << endl;
//    for (auto& vm_id : vm_ids_) {
//        cout << "DDD " << vm_id << ">>";
//        for (auto& id : vcpu_ids_[vm_id]) {
//            cout << ":" << id;
//        }
//        cout << endl;
//    }
    has_data_ = true;
    return;

}

void VmCpuUsage::refresh_system()
{
    unsigned long long user_time;
    unsigned long long nice_time;
    unsigned long long sys_time;
    unsigned long long idle_time;

    FILE* fp = fopen(SYS_PROC_STAT, "r");
    if (fp == nullptr) {
        THROW(FileOpenFailed, "To open file: " + string(SYS_PROC_STAT));
        return;
    }
    fscanf(fp, "%*s %llu %llu %llu %llu", &user_time, &nice_time,
            &sys_time, &idle_time);
    total_cpu_time_old = total_cpu_time_new;
    total_cpu_time_new = user_time + nice_time + sys_time + idle_time;
    total_cpu_time_delta = total_cpu_time_new - total_cpu_time_old;
    idle_time_old = idle_time_new;
    idle_time_new = idle_time;
    idle_time_delta = idle_time_new - idle_time_old;
    fclose(fp);
}

void VmCpuUsage::refresh_vm()
{
    VmBase* vm_base = VmBase::get_instance();
    set<VmId> vm_ids = vm_base->get_vm_ids();
    pid_t* vm_pids = new pid_t[vm_ids.size() + 1];
    int idx = 0;
    for (auto& vm_id : vm_ids) {
        vm_pids[idx] = vm_id.pid;
        ++idx;
    }
    vm_pids[idx] = 0;

    PROCTAB* proctab = openproc(READ_PROC_FLAG, vm_pids); 
    proc_t proc_info;
    memset(&proc_info, 0, sizeof(proc_info));
    proc_t task_info;
    memset(&task_info, 0, sizeof(task_info));
    // invalid old data
    for_each(vm_threads_.begin(), vm_threads_.end(), [](std::pair<const pid_t, VmThread>& p) { p.second.valid_ = false; });


    vms_.clear();
    while (readproc(proctab, &proc_info) != nullptr) {
        VmId vm_id;
        for (auto& vi : vm_ids) {
            if (vi.pid == proc_info.tid) {
                vm_id = vi;
                break;
            }
        }
        if (vm_id.pid == 0) //not find in VmBase, it means this VM is just started and has not been detected by VmBase;
            continue;
        vms_[vm_id] = 0;
        int cnt = 0;
        while (readtask(proctab, &proc_info, &task_info) != nullptr) {
            vm_threads_[task_info.tid].update(&task_info);
            vm_threads_[task_info.tid].valid_ = true;
            vms_[vm_id] += vm_threads_[task_info.tid].proctime_new_ - vm_threads_[task_info.tid].proctime_old_;
            ++cnt;
        }        
//        cout << "DDD:" << vm_id << ":" << vms_[vm_id] << endl;
        vms_[vm_id] = (total_cpu_time_old == 0 || total_cpu_time_delta == 0) ?
            0 : (vms_[vm_id] / cnt * 100 * cpu_num_ / total_cpu_time_delta);
    }        

    // erase data of invalid proc
    for (auto& p : vm_threads_) {
        if (!p.second.valid_) {
            p.second.clear_data();
        }
    }

    closeproc(proctab);
    delete[] vm_pids;
}

void VmCpuUsage::run()
{
    while(!stop_)
    {
        refresh();
        this_thread::sleep_for(chrono::milliseconds(ms_interval_));
    }
}

VmThread::VmThread():
    valid_(false),proctime_new_(0),proctime_old_(0)
{
}

void VmThread::update(const proc_t* task)
{
    processor_ = task->processor;
    tgid_ = task->tgid;
    if(proctime_new_ == 0)
    {
        proctime_new_ = task->utime + task->stime;
        proctime_old_ = proctime_new_;
    }
    else
    {
        proctime_old_ = proctime_new_;
        proctime_new_ = task->utime + task->stime;
    }
    auto vm_cpu_usage = VmCpuUsage::get_instance();
    unsigned int sys_cpu = vm_cpu_usage->get_total_cpu_time_delta();
    unsigned int proctime_delta = proctime_new_ - proctime_old_;
    cpu_usage_ = (sys_cpu == 0) ? 
        0 : (proctime_delta * 100 * VmCpuUsage::cpu_num_
            / sys_cpu);

//    cout << "TTT" << task->tid << ":" << processor_ << ":" << tgid_ << ":" << cpu_usage_ << endl;
//    cout << "sys_cpu:" << sys_cpu << " proctime_delta:" << proctime_delta << " num_cpus:" << VmCpuUsage::cpu_num_ << endl;
//    cout << "proctime_old_:" << proctime_old_ << " proctime_new_:" << proctime_new_ << " utime:" << task->utime << " stime:" << task->stime << endl;

}

void VmThread::clear_data()
{
    proctime_new_ = 0;
    proctime_old_ = 0;
    cpu_usage_ = 0;
}
