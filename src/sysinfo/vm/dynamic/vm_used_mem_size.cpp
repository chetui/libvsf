#include"sysinfo\vm\dynamic\vm_used_mem_size.h
#include"sysinfo\vm\dynamic\vm_numa_info.h
void vmUsedMemSize::update()
{
	unique_lock<shared_timed_mutex> lock(mlock);
	max_node=numa_max_node();
	int ret=get_proc_numa_dist(pid,pages,max_nodes+1);
	return ret;
}
void vmUsedMemSize::run()
{
	while(!stop_)
	{
		update();
		usleep(usleep_interval);
	}
}

vector<uint64_t> vmUsedMemSize::get_used_mem_size()
{
	if(!init)
		update();
	shared_lock<shared_timed_mutex> lock(mlock);
	return vector<uint64_t>(pages,pages+max_node);
}

uint64_t vmUsedMemSize::get_used_mem_size(int nodeid)
{
	if(!init)
		update();
	shared_lock<shared_timed_mutex> lock(mlock);
	return pages[nodeid];
}
