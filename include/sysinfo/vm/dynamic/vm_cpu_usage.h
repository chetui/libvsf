#ifndef _VM_CPU_USAGE_H_
#define _VM_CPU_USAGE_H_

#include <unistd.h>
#include <proc/readproc.h>
#include <set>
#include <map>
#include <sys/types.h>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include "../../../utils/runnable.h"
#include "sysinfo/vm/dynamic/vm_base.h"
#include "sysinfo/host/static/node_cpu.h"

struct VmThread;

class VmCpuUsage : public Runnable {
    friend class VmThread;
public:
    static VmCpuUsage *get_instance();
    void set_interval(int interval_ms);
    int get_sys_cpu_usage();
    int get_cpu_usage(VmId vm_id);
    int get_cpu_usage(pid_t vmthread_id);
    HpthreadId get_running_on_hpthread(pid_t vmthread_id);

private:
    VmCpuUsage();
    ~VmCpuUsage();
    void run();
    void refresh();
    void refresh_system();
    void refresh_vm();
    void refresh_and_wait();
    unsigned int get_total_cpu_time_delta() const;

    std::map<pid_t, VmThread> vm_threads_;
    std::map<VmId, unsigned int> vms_;

    std::shared_timed_mutex data_mutex_;

    unsigned int total_cpu_time_new = 0;
    unsigned int total_cpu_time_old = 0;
    unsigned int total_cpu_time_delta = 0;
    unsigned int idle_time_old = 0;
    unsigned int idle_time_new = 0;
    unsigned int idle_time_delta = 0;
    std::atomic<bool> has_data_;
    std::atomic<int> interval_ms_;
    static unsigned int cpu_num_;

    static constexpr const char * SYS_PROC_STAT = "/proc/stat";
    static constexpr const int READ_PROC_FLAG = PROC_FILLSTAT|PROC_PID;
};

struct VmThread {

    VmThread();
    /**
     * Update internal data.
     */
    void update(const proc_t* task);
    /**
     * Get CPU usage of this task.
     * @return percentage of CPU usage.
     */
    void clear_data();

    bool valid_;
    unsigned long long proctime_new_;
    unsigned long long proctime_old_;
    unsigned int cpu_usage_;
    int processor_;
    int tgid_;
};

#endif
