#ifndef _PERF_EVENT_H_
#define _PERF_EVENT_H_

#include <sys/types.h>
#include <linux/perf_event.h>

#include <cstdint>

#if defined(__i386__)
#define rmb()       asm volatile("lock; addl $0,0(%%esp)" ::: "memory")
#define cpu_relax() asm volatile("rep; nop" ::: "memory");
#endif
#if defined(__x86_64__)
#define rmb()       asm volatile("lfence" ::: "memory")
#define cpu_relax() asm volatile("rep; nop" ::: "memory");
#endif

namespace perf_event {

struct perf_mmap {
    int prev;
    void* base;
    int mask;
};

/**
 * System call wraper
 */
long perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu,
        int group_fd, unsigned long flags);

int mmap_buffer_read(struct perf_event_mmap_page* header, void* buf, size_t size, int page_num); 

void perf_mmap_read(struct perf_mmap* mmap_data, int fd, int page_num);

void mmap_buffer_skip(struct perf_event_mmap_page* header, int size);

uint64_t perf_mmap__read_head(struct perf_mmap* mm);

}

#endif
