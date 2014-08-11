#include <vector>
#include <string.h>
#include "sysinfo/numa_maps.h"
#include "numasche/numainfo.h"

using namespace std;

int get_proc_numa_dist(int pid,uint64_t *pages,int nnodes)
{
	vector<MemBlock> mem_blocks;
	NUMAMaps::get_mem_blocks(pid, &mem_blocks);
	
	memset(pages, 0, sizeof(uint64_t) * nnodes);
	for (auto& block : mem_blocks)
	{
		for(int i = 0; i < nnodes; i++)
            if(block.num_pages_on_node[i]>=0)
			    pages[i] += block.num_pages_on_node[i];
	}
	return 0;
}
