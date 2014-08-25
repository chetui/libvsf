#ifndef _CACHE_MISS_H_
#define _CACHE_MISS_H_

#include <map>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include "utils/runnable.h"
#include "utils/perf_event.h"

class CacheMissData;

class CacheMiss : public Runnable {
public:
    static CacheMiss *get_instance();

    void set_loop_interval(int interval_ms);
    void set_sample_interval(int interval_ms);

    CacheMissData get_cache_miss(pid_t pid);

    void start_watching(pid_t pid);
    void stop_watching(pid_t pid);
    void start_sample();
    void stop_sample();

private:
    CacheMiss();
    ~CacheMiss();
    void clear();
    void refresh();
    void run();

    std::map<pid_t, CacheMissData> cache_miss_data_;
    std::vector<pid_t> to_start_watching_;
    std::vector<pid_t> to_stop_watching_;

    std::shared_timed_mutex data_mutex_;

    std::atomic<bool> has_data_;
    std::atomic<int> loop_interval_ms_;
    std::atomic<int> sample_interval_us_;
};

class CacheMissData {
    friend class CacheMiss;
public:
    CacheMissData();
    CacheMissData(pid_t pid);
    CacheMissData(const CacheMissData& c);
    CacheMissData& operator=(const CacheMissData& c);
    pid_t pid = -1;
    /**
     * Miss per thousand cycles
     */
    double mptc = -1.0;
    /**
     * Miss per thousand instructions
     */
    double mpti = -1.0;
    /**
     * Miss per thousand references
     */
    double mptr = -1.0;
    /**
     * Reference per thousand instructions
     */
    double rpti = -1.0;
    /**
     * Cycle per instruction
     */
    double cpi = -1.0;

    class PerfData {
        friend class CacheMissData;
    public:
        unsigned long long count = 0;
    private:
        PerfData(unsigned long long c);
        PerfData(const PerfData& pd);
        PerfData& operator=(const PerfData& pd);
        bool open_fd(pid_t pid);
        bool open_fd(pid_t pid, int fd_dep);
        bool first_read();
        bool second_read();
        void clear_data();
        void clear_fd();

        int fd_ = -1;
        struct perf_event_attr attr_;
        unsigned long long first_time_ = 0;
        unsigned long long second_time_ = 0;
    };
    PerfData cache_misses;
    PerfData cpu_cycles;
    PerfData instructions;
    PerfData cache_references;
private:
    bool open_fd();
    bool first_read();
    bool second_read();
    void clear_fd();
    void clear_data();

};


#endif
