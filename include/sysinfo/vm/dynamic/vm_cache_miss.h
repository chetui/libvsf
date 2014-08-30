#ifndef _VM_CACHE_MISS_H_
#define _VM_CACHE_MISS_H_

#include <map>
#include <set>
#include <atomic>
#include "utils/runnable.h"
#include "sysinfo/vm/dynamic/vm_base.h"
#include "sysinfo/vm/dynamic/cache_miss.h"

class VmCacheMiss : public Runnable {
    friend class CacheMiss;
public:
    static VmCacheMiss* get_instance();

    void set_loop_interval(int interval_ms);
    void set_sample_interval(int interval_us);

    CacheMissData get_cache_miss(VmId vm_id);
    CacheMissData get_cache_miss(pid_t vmthread);

private:
    VmCacheMiss();
    void refresh();
    void clear();
    void run();
    static void cal_vm_miss_rate(const CacheMissData& data);

    VmBase *vm_base;
    CacheMiss *cache_miss;

    std::map<VmId, CacheMissData> vm_cache_miss_data_;
    std::set<pid_t> last_pids_;
    //only be used by the callback func of CacheMiss
    std::map<pid_t, VmId> volatile_vmthread_id_to_vm_id_;
    std::atomic<int> loop_interval_ms_;
};

#endif
