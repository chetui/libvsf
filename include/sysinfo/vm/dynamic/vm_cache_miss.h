#ifndef _VM_CACHE_MISS_H_
#define _VM_CACHE_MISS_H_

#include <map>
#include <set>
#include <atomic>
#include <memory>
#include "utils/runnable.h"
#include "sysinfo/vm/dynamic/vm_base.h"
#include "sysinfo/vm/dynamic/cache_miss.h"

class VmCacheMiss : public Runnable {
    friend class CacheMiss;
public:
    static VmCacheMiss* get_instance();

    void set_loop_interval(int interval_ms);
    void set_sample_interval(int interval_us);
    void set_callback(cache_miss_callback_t callback_func);
    void clear_param();

    CacheMissData get_cache_miss(VmId vm_id);
    CacheMissData get_cache_miss(pid_t vmthread_id);

    static constexpr const int DEFAULT_LOOP_INTERVAL_MS = 1000;
    static constexpr const int DEFAULT_SAMPLE_INTERVAL_MS = CacheMiss::DEFAULT_SAMPLE_INTERVAL_MS;

private:
    VmCacheMiss();
    ~VmCacheMiss();
    void refresh();
    void clear();
    void run();
    static void cal_vm_miss_rate(const CacheMissData& data);

    VmBase *vm_base_;
    CacheMiss *cache_miss_;

    std::map<VmId, CacheMissData> vm_cache_miss_data_;
    std::set<pid_t> last_tids_;
    //only be used by the callback func of CacheMiss
    std::map<pid_t, VmId> volatile_vmthread_id_to_vm_id_;

    std::atomic<int> loop_interval_ms_;
    std::unique_ptr<cache_miss_callback_t> callback_func_;
};

#endif
