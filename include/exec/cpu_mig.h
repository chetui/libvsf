#ifndef _CPU_MIG_H_
#define _CPU_MIG_H_

#include <map>
#include <set>
#include <mutex>

class CpuMig {
public:
    static CpuMig* get_instance();

    void set_cpu_mig(pid_t pid, std::set<int>& cpu_ids);
    void exec_cpu_mig();
private:
    CpuMig();
    ~CpuMig();
    std::map<pid_t, std::set<int> > cpu_migs_;
    std::mutex data_mutex_;
    int cpu_num_;
};

#endif
