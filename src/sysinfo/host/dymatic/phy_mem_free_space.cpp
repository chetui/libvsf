
#include <vector>
#include <iostream>
#include <exception>
#include <atomic>
#include <unordered_map>
#include <unistd.h>
#include <numa.h>
#include <mutex>
#include <shared_mutex>
#include "sysinfo/host/dynamic/phy_mem_free_space.h"

using namespace std;

phy_mem_free_space* phy_mem_free_space::get_instance()
{
    static phy_mem_free_space instance;
    return &instance;
}

int phy_mem_free_space::get_numa_free_space_info()
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

void phy_mem_free_space::run()
{
    get_numa_free_space_info();
    thread_run=true;
    while(1)
    {
        get_numa_free_space_info();
        usleep(sleep_interval_us);
    }
}

void phy_mem_free_space::reflesh()
{
    get_numa_free_space_info();
}

long long phy_mem_free_space::get_used_mem_size()
{
    if(!thread_run)
        reflesh();
    shared_lock<shared_timed_mutex> lock(thread_mutex);
    return total_mem-total_free;
}

long long phy_mem_free_space::get_used_mem_size(int node_id)
{
    shared_lock<shared_timed_mutex> lock(thread_mutex);
    unordered_map<int,mem_info>::iterator it=mem_infos.find(node_id);
    if(it==mem_infos.end())
        throw string("Node id is invalid!\n");
    return it->second.mem_total-it->second.mem_free;
}



int main()
{
    phy_mem_free_space *p=phy_mem_free_space::get_instance();
    cout<<"memory:"<<p->get_used_mem_size()<<endl;
    for(int i=0;i<p->node_num;i++)
        cout<<"node"<<i<<" memory:"<<p->get_used_mem_size(i)<<endl;
    return 0;
}