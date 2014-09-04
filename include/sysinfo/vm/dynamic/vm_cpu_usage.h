#ifndef _VM_CPU_USAGE_H_
#define _VM_CPU_USAGE_H_

#include <set>
#include <atomic>
#include "../../../utils/runnable.h"
#include "sysinfo/vm/dynamic/cpu_usage.h"
#include "sysinfo/vm/dynamic/vm_base.h"
#include "sysinfo/host/static/node_cpu.h"

class VmCpuUsage : public Runnable {
public:
    static VmCpuUsage *get_instance();

    void set_interval(int interval_ms);
    void set_callback(cpu_usage_callback_t callback_func);

    int get_sys_cpu_usage();
    int get_cpu_usage(VmId vm_id);
    int get_cpu_usage(pid_t vmthread_id);
    HpthreadId get_running_on_hpthread(pid_t vmthread_id);

private:
    VmCpuUsage();
    ~VmCpuUsage();
    void run();
    void refresh();
    void refresh_twice();
    void clear();
    static void cpu_usage_callback(pid_t pid, pid_t tid, int cpu_usage);

    std::set<pid_t> last_pids_;
    std::atomic<int> interval_ms_;
    std::atomic<cpu_usage_callback_t*> callback_func_;

    CpuUsage* cpu_usage_;
    VmBase* vm_base_;
};

#endif
