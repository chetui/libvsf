#ifndef __NUMAINFO_H__
#define __NUMAINFO_H__

#ifdef __cplusplus
extern "C" {
#endif

int get_proc_numa_dist(int pid,uint64_t *pages,int nnodes);

#ifdef __cplusplus
}
#endif

#endif
