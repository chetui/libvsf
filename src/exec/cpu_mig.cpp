#include "exec/cpu_mig.h"

#include <unistd.h>
#include "framework/exception.h"

CpuMig::CpuMig()
{
    cpu_num_ = sysconf(_SC_NPROCESSORS_CONF);
}

CpuMig::~CpuMig()
{
}

CpuMig* CpuMig::get_instance()
{
    static CpuMig cpu_mig;
    return &cpu_mig;
}

void CpuMig::set_cpu_mig(pid_t pid, std::set<int>& cpu_ids)
{
    std::unique_lock<std::mutex> lock(data_mutex_);
    cpu_migs_[pid] = cpu_ids;
}

void CpuMig::exec_cpu_mig()
{
    std::unique_lock<std::mutex> lock(data_mutex_);

    cpu_set_t* mask = CPU_ALLOC(cpu_num_);
    if (mask == nullptr) {
        THROW(CpuAllocFailed, "exec_cpu_mig. ");
    }
    size_t size = CPU_ALLOC_SIZE(cpu_num_);

    for (auto& cpu_mig : cpu_migs_) {
        CPU_ZERO_S(size, mask);
        for (auto& cpu_id : cpu_mig.second) {
            CPU_SET(cpu_id, mask);
        }
        if (sched_setaffinity(cpu_mig.first, size, mask) < 0) {
            LDEBUG << "exec_cpu_mig:sched_setaffinity failed on pid:" << cpu_mig.first;
        }
    }

    cpu_migs_.clear();
    CPU_FREE(mask);
    return;
}
