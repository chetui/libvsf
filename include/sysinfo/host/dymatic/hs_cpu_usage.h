#ifndef __HS_CPU_USAGE_H__
#define __HS_CPU_USAGE_H__ "Fuckyu_my_baby!"

#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include "../../../utils/runnable.h"

using namespace std;


class hsCpuUsage:public Runnable
{
    int cpu_num;
    double total_cpu_usage;
    int int_time_us;
    int relax_time_us;
    unordered_map<int,double> cpu_usages;
    atomic<bool> init;
    shared_timed_mutex rw_lock;
    hsCpuUsage(int int_time_us=300000,int relax_time_us=1000000):cpu_num(0),\
    total_cpu_usage(-1),int_time_us(int_time_us),relax_time_us(relax_time_us){}
    int collect_cpu_usage();
public:
    static hsCpuUsage *get_instance();
    double get_cpu_usage();
    double get_cpu_usage(int cpuid);
    int set_cpu_usage_collect_interval_us(int time_us);
    int set_sleep_interval_us(int time_us);
    void run(); 
    void refresh();
};
#endif
