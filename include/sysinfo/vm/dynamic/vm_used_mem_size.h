#ifndef VM_USED_MEM_SIZE_H
#define VM_USED_MEM_SIZE_H

#include<atomic>
#include<mutex>
#define MAX_NODE 1024
class vm_used_mem_size
{
	int max_node;
	int pid;
	int usleep_interval;
	uint64_t pages[MAX_NODE];
	atomic<bool> init; 
	mutex mlock;
public:
	vm_used_mem_size(int pid,int usleep_interval=1000000):pid(pid),init(false),usleep_interval(usleep_interval){}
	void run();
	void update();
	vector<uint64_t> get_used_mem_size();
	uint64_t get_used_mem_size(int nodeid);
}

#endif
