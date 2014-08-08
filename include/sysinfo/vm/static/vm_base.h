#ifndef _VM_BASE_
#define _VM_BASE_

#include <ctime>
#include <set>
#include <map>
#include <string>
#include "../../../utils/runnable.h"

struct VmId;

class VmBase : public Runnable {
public:
    static VmBase *get_instance();
    void set_vm_cmd(std::string vm_cmd);
    std::set<VmId> get_vm_ids();
    int get_vm_total_mem_size(VmId vm_id);

private:
    VmBase();
    ~VmBase();
    void run();
    void refresh();

    //TODO thread safty
    std::set<VmId> vm_ids_;
    std::map<VmId, int> vm_total_mem_size_;
    unsigned long long pid_max_;
    char *buf_;
    std::string vm_cmd_ = "qemu-system-x86_64";

    static constexpr const int BUF_SIZE = 102400;
    static constexpr const char* PID_MAX_FILE = "/proc/sys/kernel/pid_max";
};

struct VmId {
    VmId() {};
    VmId(time_t start_timestamp, unsigned long long pid): 
        start_timestamp(start_timestamp), pid(pid) {}
    time_t start_timestamp;
    unsigned long long pid;
};

bool operator==(const VmId &lv, const VmId &rv);
bool operator<(const VmId &lv, const VmId &rv);

#endif
