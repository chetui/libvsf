#ifndef __TOTAL_MEM_SIZE_H__
#define __TOTAL_MEM_SIZE_H__ 

#include"sysinfo/host/static/total_mem_size.h"
#include<numa.h>
totalMemSize* totalMemSize::get_instance()
{
	static totalMemSize instance;
	return &instance;
}

int totalMemSize::get_phy_node_mem()
{
	vector<phy_node> _node_info;
	long long _total_mem=0;
	int max_node=numa_max_node();
	for(int i=0;i<=max_node;i++)
	{
		long long cur_mem=numa_node_size64(i,NULL);
		if(cur_mem>=0)
		{
			_node_info.push_back(phy_node(i,cur_mem));
			_total_mem+=cur_mem;
		}
	}
	unique_lock<shared_timed_mutex> lock(mtx);
	total_mem=_total_mem;
	node_info=_node_info;
	init=true;
    return 0;
}

void totalMemSize::update()
{
	get_phy_node_mem();
}

long long totalMemSize::get_total_mem_size()
{
	if(!init)
		update();
	shared_lock<shared_timed_mutex> lock(mtx);
	return total_mem;
}

long long totalMemSize::get_total_mem_size(int node_id)
{
	if(!init)
		update();
	shared_lock<shared_timed_mutex> lock(mtx);
	for(auto &elem:node_info)
	{
		if(elem.node_id==node_id)
			return elem.mem_size;
	}
	return -1;
}
#endif
