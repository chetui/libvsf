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
    pid_t get_vcpu_num(VmId vm_id);
    std::set<pid_t> get_stable_vmthread_ids(VmId vm_id);
    pid_t get_stable_vmthread_num(VmId vm_id);

private:
    VmBase();
    ~VmBase();
    void run();
    void refresh_most();
    void refresh_vcpu();

    std::set<VmId> vm_ids_;
    std::map<VmId, std::string> name_;
    std::map<VmId, std::string> uuid_;
    std::map<VmId, int> vsocket_num_;
    std::map<VmId, int> vcore_num_;
    std::map<VmId, int> vhpthread_num_;
    std::map<VmId, int> total_mem_size_;
    std::mutex vm_ids_mutex_;
    std::mutex name_mutex_;
    std::mutex uuid_mutex_;
    std::mutex vsocket_num_mutex_;
    std::mutex vcore_num_mutex_;
    std::mutex vhpthread_num_mutex_;
    std::mutex total_mem_size_mutex_;

    std::map<VmId, std::set<pid_t>> vcpu_ids_;
    std::map<VmId, std::set<pid_t>> stable_vmthread_ids_;
    std::mutex vcpu_ids_mutex_;
    std::mutex stable_vmthread_ids_mutex_;

//    std::map<VmId, std::string> volatile_vmthread_ids_; //proc
//
    char *buf_;
    std::string vm_cmd_ = "qemu-system-x86_64";
    std::atomic<bool> has_data_most;
    std::atomic<bool> has_data_vcpu;

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
