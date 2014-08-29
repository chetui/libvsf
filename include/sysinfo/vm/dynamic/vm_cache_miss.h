#ifndef _VM_CACHE_MISS_H_
#define _VM_CACHE_MISS_H_

#include <map>
#include <atomic>
#include "utils/runnable.h"
#include "sysinfo/vm/dynamic/vm_base.h"
#include "sysinfo/vm/dynamic/cache_miss.h"

class VmCacheMiss : public Runnable {
    friend class CacheMiss;
public:
    static VmCacheMiss* get_instance();

    void set_interval(int interval_ms);

    CacheMissData get_cache_miss(Vmid vm_id);
    CacheMissData get_cache_miss(Vmid vm_id, pid_t vmthread);

private:
    VmCacheMiss();
    void run();
    void cal_vm_miss_rate(const CacheMissData& data);

    VmBase *vm_base;
    CacheMiss *cache_miss;

    std::map<VmId, CacheMissDate> vm_cache_miss_data_;
    atomic<int> loop_interval_ms;
};

#endif
