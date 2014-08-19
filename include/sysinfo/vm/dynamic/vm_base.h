#ifndef _VM_BASE_
#define _VM_BASE_

#include <ctime>
#include <set>
#include <map>
#include <sys/types.h>
#include <string>
#include <iostream>
#include <mutex>
#include <atomic>
#include "../../../utils/runnable.h"
#include "../../../utils/lock_proxy.h"

struct VmId;

class VmBase : public Runnable {
public:
    static VmBase *get_instance();
    void set_vm_cmd(std::string vm_cmd);
    std::set<VmId> get_vm_ids();
    std::set<VmId> get_vm_ids(std::string vm_cmd);
    std::string get_name(VmId vm_id);
    std::string get_uuid(VmId vm_id);
    int get_vsocket_num(VmId vm_id);
    int get_vcore_num(VmId vm_id);
    int get_vhpthread_num(VmId vm_id);
    int get_total_mem_size(VmId vm_id);
    std::set<pid_t> get_vcpu_ids(VmId vm_id);
    int get_vcpu_num(VmId vm_id);
    std::set<pid_t> get_stable_vmthread_ids(VmId vm_id);
    int get_stable_vmthread_num(VmId vm_id);
    std::set<pid_t> get_volatile_vmthread_ids(VmId vm_id);
    int get_volatile_vmthread_num(VmId vm_id);

private:
    VmBase();
    ~VmBase();
    void run();
    void refresh();
    std::set<pid_t> refresh_volatile_vmthread(VmId vm_id);

    ReadWriteLock<std::set<VmId> > vm_ids_;
    ReadWriteLock<std::map<VmId, std::string> > name_;
    ReadWriteLock<std::map<VmId, std::string> > uuid_;
    ReadWriteLock<std::map<VmId, int> > vsocket_num_;
    ReadWriteLock<std::map<VmId, int> > vcore_num_;
    ReadWriteLock<std::map<VmId, int> > vhpthread_num_;
    ReadWriteLock<std::map<VmId, int> > total_mem_size_;

    ReadWriteLock<std::map<VmId, std::set<pid_t> > > vcpu_ids_;
    ReadWriteLock<std::map<VmId, std::set<pid_t> > > stable_vmthread_ids_;

    char *buf_;
    std::string vm_cmd_ = "qemu-system-x86_64";
    std::atomic<bool> has_data;

    static constexpr const int BUF_SIZE = 102400;
    static constexpr const char * VCPU_DIR = "/sys/fs/cgroup/cpu/libvirt/qemu/";
//    pid_t pid_max_;
//    static constexpr const char* PID_MAX_FILE = "/proc/sys/kernel/pid_max";
};

struct VmId {
    VmId() {};
    VmId(std::time_t start_timestamp, pid_t pid): 
        start_timestamp(start_timestamp), pid(pid) {}
    std::time_t start_timestamp;
    pid_t pid;
};

bool operator==(const VmId &lv, const VmId &rv);
bool operator<(const VmId &lv, const VmId &rv);
std::ostream &operator<<(std::ostream &os, const VmId &v);

#endif
