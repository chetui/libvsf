#include <vector>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "sysinfo/host/dynamic/phy_cpu_usage.h"

using namespace std;

long long atoll(char *buf)
{
    long long ret=0;
    int flag=1;
    if(buf==NULL)
        return 0;
    if(*buf=='-')
    {
        flag=-1;
        buf++;
    }
    while(*buf!=0&&*buf>='0'&&*buf<='9')
    {
        ret*=10;
        ret+=*buf-'0';
        buf++;
    }
    return flag*ret;
}

struct cpu_time_info_struct
{
    int cpu_id;
    long long cpu_time;
    long long cpu_idle_time;
};

struct phy_cpu_info_ret_struct
{
    int cpu_num;
    long long total_cpu_time;
    long long total_cpu_idle_time;
    vector<cpu_time_info_struct> cpu_time_infos;
    phy_cpu_info_ret_struct():cpu_num(0),total_cpu_time(0){}
};

int get_phy_cpu(phy_cpu_info_ret_struct *cpu_time_info)
{
    FILE *fp=fopen("/proc/stat","r");
    char buf[1024],*token;
    long long time=0;
    long long idle_time=0;
    int index=0;
    phy_cpu_info_ret_struct ret;
    
    if(!fgets(buf,1024,fp))
        return -1;
    
    token=strtok(buf," ");
    if(strcmp(token,"cpu")!=0)
        return -1;
    
    while(token=strtok(NULL," "))
    {
        index++;
        time+=atoll(token);
        #define INDL_TIME_INDEX 4
        if(index==INDL_TIME_INDEX)
            idle_time=atoll(token);
    }
    
    ret.total_cpu_time=time;
    ret.total_cpu_idle_time=idle_time;
    
    while(fgets(buf,1024,fp))
    {
        cpu_time_info_struct time_info;
        token=strtok(buf," ");
        if(strncmp(buf,"cpu",3)!=0)
            continue;
        time_info.cpu_id=atoll(token+3);
        time=idle_time=0;
        
        index=0;
        
        while(token=strtok(NULL," "))
        {
            index++;
            time+=atoll(token);
            #define INDL_TIME_INDEX 4
            if(index==INDL_TIME_INDEX)
                idle_time=atoll(token);
        }
        time_info.cpu_time=time;
        time_info.cpu_idle_time=idle_time;
        ret.cpu_time_infos.push_back(time_info);
    }
    ret.cpu_num=ret.cpu_time_infos.size();
    *cpu_time_info=ret;
    return 0;
}


int phy_cpu_usage::set_cpu_usage_collect_interval_us(int time_us)
{
    int_time_us=time_us;
    return 0;
}

int phy_cpu_usage::set_sleep_interval_us(int time_us)
{
    relax_time_us=time_us;
    return 0;
}

phy_cpu_usage * phy_cpu_usage::get_instance()
{
    static phy_cpu_usage instance;
    return &instance;
}

int phy_cpu_usage::collect_cpu_usage()
{
    phy_cpu_info_ret_struct ret1,ret2;
    int retval;
        
    double _total_cpu_usage;
    unordered_map<int,double> _cpu_usages;
    
    retval=get_phy_cpu(&ret1);
    if(retval<0)
        return retval;
    
    usleep(int_time_us);
    
    retval=get_phy_cpu(&ret2);
    if(retval<0)
        return retval;
    
    if(ret1.cpu_num!=ret2.cpu_num)
        return -1;
    
    _total_cpu_usage=1-(double)(ret1.total_cpu_idle_time-ret2.total_cpu_idle_time)/(ret1.total_cpu_time-ret2.total_cpu_time);
    
    for(int i=0;i<ret1.cpu_num;i++)
    {
        cpu_time_info_struct info1=ret1.cpu_time_infos[i],info2=ret2.cpu_time_infos[i];
        
        if(info1.cpu_id!=info2.cpu_id)
            return -1;
        
        double usage=1-(double)(info1.cpu_idle_time-info2.cpu_idle_time)/(info1.cpu_time-info2.cpu_time);
        _cpu_usages.insert(make_pair(info1.cpu_id,usage));
    }
    
    unique_lock<shared_timed_mutex> lock(rw_lock);
    total_cpu_usage=_total_cpu_usage;
    cpu_usages=_cpu_usages;
    init=true;
    
    return 0;
}

void phy_cpu_usage::run()
{
    while(true)
    {
        collect_cpu_usage();
        usleep(relax_time_us);
    }
}

void phy_cpu_usage::refresh()
{
    collect_cpu_usage();
}

double phy_cpu_usage::get_cpu_usage(int cpuid)
{
    if(!init)
        refresh();
    
    shared_lock<shared_timed_mutex> lock(rw_lock);
    auto it=cpu_usages.find(cpuid);
    if(it!=cpu_usages.end())
        return it->second;
    else
    {
        unsigned long long ret=0xfff7ffffffffffff;//SNAN
        return *((double *)(&ret));       
    }
}

double phy_cpu_usage::get_cpu_usage()
{
    if(!init)
        refresh();
    
    shared_lock<shared_timed_mutex> lock(rw_lock);
    return total_cpu_usage;
}
