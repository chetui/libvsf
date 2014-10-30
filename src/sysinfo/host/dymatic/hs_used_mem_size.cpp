
#include <vector>
#include <iostream>
#include <exception>
#include <atomic>
#include <unordered_map>
#include <unistd.h>
#include <numa.h>
#include <mutex>
#include <shared_mutex>
#include "sysinfo/host/dymatic/hs_used_mem_size.h"

using namespace std;

hsUsedMemSize* hsUsedMemSize::get_instance()
{
    static hsUsedMemSize instance;
    return &instance;
}

int hsUsedMemSize::get_numa_free_space_info()
{
    unique_lock<shared_timed_mutex> lock(thread_mutex);
    int max_node=numa_max_node();
    long long node_free_space;
    long long node_total_space;
    long long total_mem_space=0;
    long long total_free_space=0;
    unordered_map<int,mem_info> _mem_infos;
    for(int i=0;i<=max_node;i++)
    {
        node_total_space=numa_node_size64(i,&node_free_space);
        if(node_total_space==-1)
            continue;
        mem_info minfo;
        minfo.mem_free=node_free_space;
        minfo.mem_total=node_total_space;
        _mem_infos.insert(make_pair(i,minfo));
        total_free_space+=node_free_space;
        total_mem_space+=node_total_space;
    }
    
    mem_infos=_mem_infos;
    total_free=total_free_space;
    total_mem=total_mem_space;
    node_num=_mem_infos.size();
    return 0;
}

void hsUsedMemSize::run()
{
    get_numa_free_space_info();
    thread_run=true;
    while(!stop_)
    {
        get_numa_free_space_info();
        usleep(sleep_interval_us);
    }
}

void hsUsedMemSize::reflesh()
{
    get_numa_free_space_info();
}

long long hsUsedMemSize::get_used_mem_size()
{
	if(!stop&&!thread_run)
		usleep(sleep_interval_us);

    if(!thread_run)
        reflesh();
    shared_lock<shared_timed_mutex> lock(thread_mutex);
    return total_mem-total_free;
}

long long hsUsedMemSize::get_used_mem_size(int node_id)
{
	if(!stop&&!thread_run)
		usleep(sleep_interval_us);

    if(!thread_run)
        reflesh();
		
    shared_lock<shared_timed_mutex> lock(thread_mutex);
    unordered_map<int,mem_info>::iterator it=mem_infos.find(node_id);
    if(it==mem_infos.end())
        throw string("Node id is invalid!\n");
    return it->second.mem_total-it->second.mem_free;
}

