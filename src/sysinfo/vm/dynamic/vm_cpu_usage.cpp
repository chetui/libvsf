#include "sysinfo/vm/dynamic/vm_cpu_usage.h"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <functional>
#include "sysinfo/vm/dynamic/vm_base.h"

using namespace std;

VmCpuUsage::VmCpuUsage(): has_data_(false), ms_interval_(1000)
{
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
        //TODO throw
//        LOG(LogLevel::err) << "Procs::system_wide_refresh: "
//            << strerror(errno) << endl;
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
    // invalid old data
    for_each(vm_threads_.begin(), vm_threads_.end(), [](std::pair<const pid_t, VmThread>& p) { p.second.valid = false; });


    while (readproc(proctab, &proc_info) != nullptr) {
        vm_threads_[proc_info.tid].update(&proc_info);
        vm_threads_[proc_info.tid].valid = true;
//       LOG() << proc->tid << " " 
//           << vm_threads_[proc->tid].get_cpu_usage() << endl;
    }        
//    LOG() << "size:" << vm_threads_.size();

    // erase data of invalid proc
    for (auto& p : vm_threads_) {
        if (!p.second.valid) {
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
        cout << "--------------------" << endl;
        refresh();
        this_thread::sleep_for(chrono::milliseconds(ms_interval_));
    }
}

unsigned int VmCpuUsage::get_total_cpu_time_delta() const
{
    return total_cpu_time_delta;
}

VmThread::VmThread():
    valid(false),proctime_new(0),proctime_old(0)
{
}

void VmThread::update(const proc_t* proc)
{
    processor = proc->processor;
    tgid = proc->tgid;
    if(proctime_new == 0)
    {
        proctime_new = proc->utime + proc->stime;
        proctime_old = proctime_new;
    }
    else
    {
        proctime_old = proctime_new;
        proctime_new = proc->utime + proc->stime;
    }
    auto vm_cpu_usage = VmCpuUsage::get_instance();
    unsigned int sys_cpu = vm_cpu_usage->get_total_cpu_time_delta();
    unsigned int proctime_delta = proctime_new - proctime_old;
    int num_cpus = sysconf(_SC_NPROCESSORS_CONF);
    cpu_usage = (sys_cpu == 0) ? 
        0 : static_cast<unsigned int>(proctime_delta * 100 * num_cpus
            / sys_cpu);
    cout << "TTT" << proc->tid << ":" << processor << ":" << tgid << ":" << cpu_usage << endl;
    cout << "sys_cpu:" << sys_cpu << " proctime_delta:" << proctime_delta << " num_cpus:" << num_cpus << endl;
    cout << "proctime_old:" << proctime_old << " proctime_new:" << proctime_new << " utime:" << proc->utime << " stime:" << proc->stime << endl;

}

void VmThread::clear_data()
{
    proctime_new = 0;
    proctime_old = 0;
    cpu_usage = 0;
}
