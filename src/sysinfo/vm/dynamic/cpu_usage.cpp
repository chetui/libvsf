#include "sysinfo/vm/dynamic/cpu_usage.h"

#include <cstdio>
#include <cstring>
#include <algorithm>
#include <functional>
#include <string>
#include "utils/log.h"
#include "framework/exception.h"

using namespace std;

unsigned int CpuUsage::cpu_num_;

CpuUsage::CpuUsage(): has_data_cnt_(0), interval_ms_(1000)
{
    cpu_num_ = static_cast<unsigned int>(sysconf(_SC_NPROCESSORS_CONF));
    callback_func_ = new cpu_usage_callback_t;
    *callback_func_ = nullptr;
}

CpuUsage::~CpuUsage()
{
    stop();
    delete callback_func_;
}

CpuUsage *CpuUsage::get_instance()
{
    static CpuUsage cpu_usage;
    return &cpu_usage;
}

void CpuUsage::set_interval(int interval_ms) 
{
    interval_ms_ = interval_ms;
}

void CpuUsage::set_callback(cpu_usage_callback_t callback_func) 
{
    *callback_func_ = callback_func;
}

int CpuUsage::get_sys_cpu_usage_without_refresh()
{
    while(!has_data()) 
        this_thread::sleep_for(chrono::milliseconds(10));
    shared_lock<shared_timed_mutex> lock(data_mutex_);
    return static_cast<int>((total_cpu_time_old == 0 || total_cpu_time_delta == 0) ?
        0 : (1 - idle_time_delta * 1.0 / total_cpu_time_delta) * 100);
}

int CpuUsage::get_sys_cpu_usage()
{
    refresh_twice();
    return get_sys_cpu_usage_without_refresh();
}

int CpuUsage::get_thread_cpu_usage_without_refresh(pid_t tid)
{
    while(!has_data()) 
        this_thread::sleep_for(chrono::milliseconds(10));
    shared_lock<shared_timed_mutex> lock(data_mutex_);
    auto iter = thread_cpu_usage_data_.find(tid);
    if (iter != thread_cpu_usage_data_.end() && iter->second.valid_)
        return iter->second.cpu_usage_;
    else
        return -1;
}
int CpuUsage::get_thread_cpu_usage(pid_t tid)
{
    refresh_twice();
    return get_thread_cpu_usage_without_refresh(tid);
}

int CpuUsage::get_process_cpu_usage_without_refresh(pid_t pid)
{
    while(!has_data()) 
        this_thread::sleep_for(chrono::milliseconds(10));
    shared_lock<shared_timed_mutex> lock(data_mutex_);
    auto iter = process_cpu_usage_data_.find(pid);
    if (iter != process_cpu_usage_data_.end())
        return iter->second;
    else
        return -1;
}
int CpuUsage::get_process_cpu_usage(pid_t pid)
{
    refresh_twice();
    return get_process_cpu_usage_without_refresh(pid);
}

int CpuUsage::get_thread_running_on_cpu_without_refresh(pid_t tid)
{
    while(!has_data()) 
        this_thread::sleep_for(chrono::milliseconds(10));
    shared_lock<shared_timed_mutex> lock(data_mutex_);
    auto iter = thread_cpu_usage_data_.find(tid);
    if (iter != thread_cpu_usage_data_.end() && iter->second.valid_)
        return iter->second.processor_;
    else
        return -1;
}

int CpuUsage::get_thread_running_on_cpu(pid_t tid)
{
    refresh_twice();
    return get_thread_running_on_cpu_without_refresh(tid);
}

std::shared_timed_mutex& CpuUsage::get_data_mutex()
{
    return data_mutex_;
}

std::atomic<int>& CpuUsage::get_has_data_cnt()
{
    return has_data_cnt_;
}

bool CpuUsage::has_data()
{
    return has_data_cnt_ >= 2;
}

void CpuUsage::refresh_twice()
{
    if(!joinable())
    {
        refresh();
        this_thread::sleep_for(chrono::milliseconds(interval_ms_));
        refresh();
    }
}

unsigned int CpuUsage::get_total_cpu_time_delta() const
{
    return (total_cpu_time_old == 0)?
        0: total_cpu_time_delta;
}

void CpuUsage::start_watching(pid_t pid) 
{
    to_start_watching_.push_back(pid);
}

void CpuUsage::stop_watching(pid_t pid) 
{
    to_stop_watching_.push_back(pid);
}

void CpuUsage::clear()
{
    thread_cpu_usage_data_.clear();
    process_cpu_usage_data_.clear();
    to_start_watching_.clear();
    to_stop_watching_.clear();
    total_cpu_time_new = 0;
    total_cpu_time_old = 0;
    total_cpu_time_delta = 0;
    idle_time_old = 0;
    idle_time_new = 0;
    idle_time_delta = 0;
    has_data_cnt_ = 0;
}

void CpuUsage::refresh()
{
    unique_lock<shared_timed_mutex> lock(data_mutex_);

    refresh_system();
    refresh_watching();
    refresh_thread();

//    LDEBUG << "DDD----" << endl;
//    for (auto& vm_id : vm_ids_) {
//        LDEBUG << "DDD " << vm_id << ">>";
//        for (auto& id : vcpu_ids_[vm_id]) {
//            LDEBUG << ":" << id;
//        }
//        LDEBUG << endl;
//    }
    if (has_data_cnt_ < 2)
        ++has_data_cnt_;
    return;

}

void CpuUsage::refresh_system()
{
    unsigned long long user_time;
    unsigned long long nice_time;
    unsigned long long sys_time;
    unsigned long long idle_time;

    FILE* fp = fopen(SYS_PROC_STAT, "r");
    if (fp == nullptr) {
        THROW(FileOpenFailed, "To open file: " + string(SYS_PROC_STAT));
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

void CpuUsage::refresh_watching()
{
    for (auto pid : to_start_watching_) {
        auto ret = process_cpu_usage_data_.insert({pid, {} });
        if (!ret.second) { //insert failed
            LDEBUG << "cpu usage insert to to_start_watching_ failed on pid:" << pid << ". already existed.";
        }
    }
    to_start_watching_.clear();

    for (auto pid : to_stop_watching_) {
        auto iter = process_cpu_usage_data_.find(pid);
        if (iter != process_cpu_usage_data_.end()) {
            process_cpu_usage_data_.erase(iter);
        } else {
            LDEBUG << "cpu usage erase from to_stop_watching_ failed on pid:" << pid << ". not existed";
        }
    }
    to_stop_watching_.clear();
}

void CpuUsage::refresh_thread()
{
    pid_t* pids = new pid_t[process_cpu_usage_data_.size() + 1];
    int idx = 0;
    for (auto& data : process_cpu_usage_data_) {
        pids[idx] = data.first;
        ++idx;
    }
    pids[idx] = 0;

    PROCTAB* proctab = openproc(READ_PROC_FLAG, pids); 
    if (proctab == nullptr)
    {
        string pids_str;
        for (int i = 0; i < idx -1; ++i)
            pids_str += to_string(pids[i]) + " ";
        THROW(OpenProcFailed, "To open pids: " + pids_str);
    }

    proc_t proc_info;
    memset(&proc_info, 0, sizeof(proc_info));
    proc_t task_info;
    memset(&task_info, 0, sizeof(task_info));
    // invalid old data
    for_each(thread_cpu_usage_data_.begin(), thread_cpu_usage_data_.end(), [](std::pair<const pid_t, ThreadCpuUsageData>& p) { p.second.valid_ = false; });

    while (readproc(proctab, &proc_info) != nullptr) {
        process_cpu_usage_data_[proc_info.tid] = 0;
        int cnt = 0;
        while (readtask(proctab, &proc_info, &task_info) != nullptr) {
            thread_cpu_usage_data_[task_info.tid].update(&task_info);
            thread_cpu_usage_data_[task_info.tid].valid_ = true;
            process_cpu_usage_data_[proc_info.tid] += static_cast<int>(thread_cpu_usage_data_[task_info.tid].proctime_new_ - thread_cpu_usage_data_[task_info.tid].proctime_old_);
            ++cnt;
        }        
//        LDEBUG << "DDD:" << proc_info.tid << ":" << process_cpu_usage_data_[proc_info.tid] << endl;
        process_cpu_usage_data_[proc_info.tid] = static_cast<int>((total_cpu_time_old == 0 || total_cpu_time_delta == 0) ?
            0 : (static_cast<unsigned int>(process_cpu_usage_data_[proc_info.tid]) / static_cast<unsigned int>(cnt) * 100 * cpu_num_ / total_cpu_time_delta));
    }        

    // erase data of invalid proc
    for (auto& p : thread_cpu_usage_data_) {
        if (!p.second.valid_) {
            p.second.clear_data();
        }
    }

    for (const auto& data : thread_cpu_usage_data_) {
        if (*callback_func_) {
            (*callback_func_)(data.second.tgid_, data.first, data.second.cpu_usage_);
        }
    }

    closeproc(proctab);
    delete[] pids;
}

void CpuUsage::run()
{
    while(!stop_)
    {
        refresh();
        this_thread::sleep_for(chrono::milliseconds(interval_ms_));
    }
    unique_lock<shared_timed_mutex> lock(data_mutex_);
    clear();
}

CpuUsage::ThreadCpuUsageData::ThreadCpuUsageData()
{
}

void CpuUsage::ThreadCpuUsageData::update(const proc_t* task)
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
    auto cpu_usage = CpuUsage::get_instance();
    unsigned int sys_cpu = cpu_usage->get_total_cpu_time_delta();
    unsigned int proctime_delta = proctime_new_ - proctime_old_;
    cpu_usage_ = static_cast<int>((sys_cpu == 0) ? 
        0 : (proctime_delta * 100 * CpuUsage::cpu_num_
            / sys_cpu));

//    LDEBUG << "TTT" << task->tid << ":" << processor_ << ":" << tgid_ << ":" << cpu_usage_ << endl;
//    LDEBUG << "sys_cpu:" << sys_cpu << " proctime_delta:" << proctime_delta << " num_cpus:" << CpuUsage::cpu_num_ << endl;
//    LDEBUG << "proctime_old_:" << proctime_old_ << " proctime_new_:" << proctime_new_ << " utime:" << task->utime << " stime:" << task->stime << endl;

}

void CpuUsage::ThreadCpuUsageData::clear_data()
{
    proctime_new_ = 0;
    proctime_old_ = 0;
    cpu_usage_ = -1;
    processor_ = -1;
    tgid_ = -1;
}

std::ostream &operator<<(std::ostream &os, const CpuUsage::ThreadCpuUsageData& data)
{
    os << "[" 
        << data.valid_ << ":" 
        << data.tgid_ << ":" 
        << data.processor_ << ":" 
        << data.cpu_usage_ << ":" 
        << data.proctime_new_ << ":" 
        << data.proctime_old_ << "]";
    return os;
}
