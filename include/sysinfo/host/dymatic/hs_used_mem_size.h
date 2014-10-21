#ifndef __HS_USED_MEM_SIZE_H__
#define __HS_USED_MEM_SIZE_H__ "Fuckyu my baby!"

#include <atomic>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include "../../../utils/runnable.h"

using namespace std;

struct mem_info
{
    long long mem_free;
    long long mem_total;
};

class hsUsedMemSize:public Runnable
{
    int node_num;
    long long total_free;
    long long total_mem;
    atomic<bool> thread_run;
    shared_timed_mutex thread_mutex;
    unordered_map<int,mem_info> mem_infos;
    int sleep_interval_us;
    hsUsedMemSize(int sleep_interval_us=300000):node_num(-1),total_free(0),total_mem(0),thread_run(false),sleep_interval_us(sleep_interval_us){}
    int get_numa_free_space_info();
public:
    static hsUsedMemSize* get_instance();
    long long get_used_mem_size();
    long long get_used_mem_size(int node_id);
    void reflesh();
    void run();
};

#endif