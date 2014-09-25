#ifndef _CPU_USAGE_H_
#define _CPU_USAGE_H_

#include <unistd.h>
#include <proc/readproc.h>
#include <set>
#include <map>
#include <vector>
#include <sys/types.h>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <memory>
#include "../../../utils/runnable.h"

typedef std::function<void(pid_t pid, pid_t tid, int)> cpu_usage_callback_t;

class CpuUsage : public Runnable {
public:
    static CpuUsage *get_instance();

    void set_interval(int interval_ms);
    void set_callback(cpu_usage_callback_t);
    std::shared_timed_mutex& get_data_mutex();
    std::atomic<int>& get_has_data_cnt();
    bool has_data();

    int get_sys_cpu_usage();
    int get_thread_cpu_usage(pid_t tid);
    int get_process_cpu_usage(pid_t pid);
    int get_thread_running_on_cpu(pid_t tid);
    int get_sys_cpu_usage_without_refresh();
    int get_thread_cpu_usage_without_refresh(pid_t tid);
    int get_process_cpu_usage_without_refresh(pid_t pid);
    int get_thread_running_on_cpu_without_refresh(pid_t tid);

    //not mutex protected. need get_data_mutex() to protect
    void start_watching(pid_t pid);
    //not mutex protected. need get_data_mutex() to protect
    void stop_watching(pid_t pid);
    //not mutex protected. need get_data_mutex() to protect
    void clear();
    void clear_param();
    void refresh();
    void refresh_twice();

    static constexpr const int DEFAULT_INTERVAL_MS = 1000;

private:
    class ThreadCpuUsageData {
    public:
        ThreadCpuUsageData();
        /**
         * Update internal data.
         */
        void update(const proc_t* task);
        /**
         * Get CPU usage of this task.
         * @return percentage of CPU usage.
         */
        void clear_data();

        bool valid_ = false;
        unsigned long long proctime_new_ = 0;
        unsigned long long proctime_old_ = 0;
        int cpu_usage_ = -1;
        int processor_ = -1;;
        pid_t tgid_ = -1;
    };
    friend std::ostream &operator<<(std::ostream &os, const ThreadCpuUsageData& data);

    CpuUsage();
    ~CpuUsage();
    void run();
    void refresh_system();
    void refresh_watching();
    void refresh_thread();
    unsigned int get_total_cpu_time_delta() const;

    std::map<pid_t, ThreadCpuUsageData> thread_cpu_usage_data_;
    std::map<pid_t, int> process_cpu_usage_data_;
    std::vector<pid_t> to_start_watching_;
    std::vector<pid_t> to_stop_watching_;
    unsigned int total_cpu_time_new = 0;
    unsigned int total_cpu_time_old = 0;
    unsigned int total_cpu_time_delta = 0;
    unsigned int idle_time_old = 0;
    unsigned int idle_time_new = 0;
    unsigned int idle_time_delta = 0;
    std::atomic<int> has_data_cnt_;
    std::shared_timed_mutex data_mutex_;

    std::atomic<int> interval_ms_;
    std::unique_ptr<cpu_usage_callback_t> callback_func_;

    static unsigned int cpu_num_;
    static constexpr const char * SYS_PROC_STAT = "/proc/stat";
    static constexpr const int READ_PROC_FLAG = PROC_FILLSTAT|PROC_PID;
};

std::ostream &operator<<(std::ostream &os, const CpuUsage::ThreadCpuUsageData& data);

#endif
