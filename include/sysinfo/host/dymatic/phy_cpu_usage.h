#ifndef __PHY_CPU_USAGE_H__
#define __PHY_CPU_USAGE_H__ "Fuckyu my baby!"

#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include "../../../utils/runnable.h"

using namespace std;


class phy_cpu_usage:public Runnable
{
    int cpu_num;
    double total_cpu_usage;
    int int_time_us;
    int relax_time_us;
    unordered_map<int,double> cpu_usages;
    atomic<bool> init;
    shared_timed_mutex rw_lock;
    phy_cpu_usage(int int_time_us=300000,int relax_time_us=1000000):cpu_num(0),\
    total_cpu_usage(-1),int_time_us(int_time_us),relax_time_us(relax_time_us){}
    int collect_cpu_usage();
    void run();
    void refresh();
public:
    static phy_cpu_usage *get_instance();
    double get_cpu_usage();
    double get_cpu_usage(int cpuid);
    int set_cpu_usage_collect_interval_us(int time_us);
    int set_sleep_interval_us(int time_us);
};
#endif