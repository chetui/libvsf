#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <numa.h>


#include "migrate.h"



int mem_mig::migrate_memory(int tgid,int from_node,int to_node)
{
	struct bitmask *old_mask =  numa_allocate_nodemask();
	struct bitmask *new_mask =  numa_allocate_nodemask();
	int ret = -1;
	const int MAX_TRY = 10;

	if(!old_mask || !new_mask)
		goto memfree;

	numa_bitmask_clearall(old_mask);
	numa_bitmask_clearall(new_mask);
	numa_bitmask_setbit(old_mask,from_node);
	numa_bitmask_setbit(new_mask,to_node);
	
	for(i=0;i<MAX_TRY;i++)
		if(!numa_migrate_pages(tgid, old_mask, new_mask))
			break;
	
memfree:
	if(old_mask)
		numa_free_nodemask(old_mask);
	if(new_mask)
		numa_free_nodemask(new_mask);	
	return ret;
}

