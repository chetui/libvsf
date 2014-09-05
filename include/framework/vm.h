#ifndef _VM_
#define _VM_

#include <set>
#include <string>
#include "utils/func_option.h"
#include "sysinfo/vm/dynamic/vm_base.h"
#include "sysinfo/vm/dynamic/vm_cpu_usage.h"
#include "sysinfo/vm/dynamic/vm_cache_miss.h"
#include "exec/cpu_mig.h"
#include <mutex>

class Host;
class VM;

class VmSet {
public:
    static VmSet* get_instance();
    std::set<VM> init_vms(Host *host);
    std::set<VM> init_vms(Host *host, std::string vm_cmd);
private:
    VmSet();
    ~VmSet();
    std::mutex init_vms_mutex_;

    FuncOption *func_option_;
    VmBase *vm_base_;
    VmCpuUsage *vm_cpu_usage_;
    VmCacheMiss *vm_cache_miss_;
};

class VM {
public:
    VM(VmId vm_id);
    VM &operator=(const VM &v);
    
    //OP_VM_BASE
    VmId vm_id() const;
    std::string name() const;
    std::string uuid() const;
    int vsocket_num() const;
    int vcore_num() const;
    int vhpthread_num() const;
    int total_mem_size() const;
    std::set<pid_t> vcpu_ids() const;
    int vcpu_num() const;
    std::set<pid_t> stable_vmthread_ids() const;
    int stable_vmthread_num() const;
    std::set<pid_t> volatile_vmthread_ids() const;
    int volatile_vmthread_num() const;
    //OP_VM_CPU_USAGE
    int sys_cpu_usage() const;
    int cpu_usage() const;
    int cpu_usage(pid_t vmthread_id) const;
    HpthreadId running_on_hpthread(pid_t vmthread_id) const;
    //OP_VM_CACHE_MISS
    CacheMissData cache_miss() const;
    CacheMissData cache_miss(pid_t vmthread_id) const;

    //VCPU MIG
    void set_vcpu_mig(pid_t vmthread_id, std::set<HpthreadId> hpthread_ids) const;

private:
    VmId vm_id_;

    VmBase *vm_base_;
    VmCpuUsage *vm_cpu_usage_;
    VmCacheMiss *vm_cache_miss_;
    CpuMig *cpu_mig_;
};

bool operator<(const VM &lv, const VM &rv);

#endif
