#include "utils/perf_event.h"

#include <unistd.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <sys/mman.h>

#include <cstring>

using namespace std;

long perf_event::perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu,
        int group_fd, unsigned long flags)
{
    return syscall(__NR_perf_event_open, hw_event, pid, cpu,
            group_fd, flags);
}

int perf_event::mmap_buffer_read(struct perf_event_mmap_page* header, void* buf, size_t size, int page_num) 
{
    unsigned int page_size = sysconf(_SC_PAGE_SIZE);
    void* data = (uint8_t*)header + page_size;
    uint64_t data_tail = header->data_tail;
    uint64_t data_head = header->data_head;
    rmb();
    size_t data_size, ncopies;
    if ((data_size = data_head - data_tail) < size) {
        return -1;
    }
    uint64_t mask = page_num * page_size - 1;  
    unsigned int mmap_len = (page_num + 1) * page_size;
    data_tail &= mask;

    if ((ncopies = (mmap_len - data_tail)) < size) {
        memcpy(buf, (uint8_t*)data + data_tail, ncopies);
        memcpy((uint8_t*)buf + ncopies, data, size - ncopies);
    } else {
        memcpy(buf, (uint8_t*)data+data_tail, size);
    }
    header->data_tail += size;
    return 0;
}

void perf_event::perf_mmap_read(struct perf_mmap* mmap_data, int fd, int page_num)
{
    unsigned int page_size = sysconf(_SC_PAGE_SIZE);
    unsigned int mmap_len = (page_num + 1) * page_size;
    mmap_data->base = mmap(nullptr, mmap_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mmap_data->base == MAP_FAILED) {
        //TODO: throw
//        LOG(LogLevel::err) << __FILE__ << "::perf_mmap_read:" 
//            << strerror(errno) << endl;
        close(fd);
    }
}

void perf_event::mmap_buffer_skip(struct perf_event_mmap_page* header, int size)
{
    size_t data_head = header->data_head;
    rmb();
    if ((header->data_tail+size) > data_head) {
        size = data_head - header->data_tail;
    }
    header->data_tail += size;
}

uint64_t perf_event::perf_mmap__read_head(struct perf_mmap* mm)
{
    struct perf_event_mmap_page *pc = (struct perf_event_mmap_page*) mm->base;
    uint64_t head = pc->data_head;
    rmb();
    return head;
}
