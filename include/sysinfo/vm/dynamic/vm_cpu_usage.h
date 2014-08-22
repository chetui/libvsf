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

struct VmThread;

class VmCpuUsage : public Runnable {
    friend class VmThread;
public:
    static VmCpuUsage *get_instance();
    void set_interval(int ms_interval);

private:
    VmCpuUsage();
    ~VmCpuUsage();
    void run();
    void refresh();
    void refresh_system();
    void refresh_vm();
    unsigned int get_total_cpu_time_delta() const;

    std::map<pid_t, VmThread> vm_threads_;

    std::shared_timed_mutex data_mutex_;

    unsigned int total_cpu_time_new;
    unsigned int total_cpu_time_old;
    unsigned int total_cpu_time_delta;
    unsigned int idle_time_old;
    unsigned int idle_time_new;
    unsigned int idle_time_delta;
    std::atomic<bool> has_data_;
    std::atomic<int> ms_interval_;

    static constexpr const char * SYS_PROC_STAT = "/proc/stat";
    static constexpr const int READ_PROC_FLAG = PROC_FILLSTAT|PROC_PID;
};

struct VmThread {

    VmThread();
    /**
     * Update internal data.
     */
    void update(const proc_t* proc); 
    /**
     * Get CPU usage of this task.
     * @return percentage of CPU usage.
     */
    void clear_data();

    bool valid;
    unsigned long long proctime_new;
    unsigned long long proctime_old;
    unsigned int cpu_usage;
    int processor;
    int tgid;
};

#endif
