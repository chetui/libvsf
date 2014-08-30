#ifndef _VM_BASE_
#define _VM_BASE_

#include <ctime>
#include <set>
#include <map>
#include <sys/types.h>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include "../../../utils/runnable.h"

struct VmId;

class VmBase : public Runnable {
public:
    static VmBase *get_instance();
    void set_vm_cmd(std::string vm_cmd);
    void set_interval(int interval_ms);
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

    VmId stable_vmthread_id_to_vm_id(pid_t pid);

private:
    VmBase();
    ~VmBase();
    void run();
    void refresh();
    void refresh_most();
    void refresh_vcpu_stable_vmthread();
    std::set<pid_t> refresh_volatile_vmthread(VmId vm_id);
    template <typename T>
    T get_data_by_vm_id(std::map<VmId, T>& data, VmId vm_id, const T& failed_ret);

    std::set<VmId> vm_ids_;
    std::map<pid_t, VmId> stable_vmthread_id_to_vm_id_;
    std::map<VmId, std::string> name_;
    std::map<VmId, std::string> uuid_;
    std::map<VmId, int> vsocket_num_;
    std::map<VmId, int> vcore_num_;
    std::map<VmId, int> vhpthread_num_;
    std::map<VmId, int> total_mem_size_;
    std::map<VmId, std::set<pid_t> > vcpu_ids_;
    std::map<VmId, std::set<pid_t> > stable_vmthread_ids_;

    std::shared_timed_mutex data_mutex_;

    char *buf_;
    std::string vm_cmd_ = "qemu-system-x86_64";
    std::atomic<bool> has_data_;
    std::atomic<int> interval_ms_;

    static constexpr const int BUF_SIZE = 102400;
    static constexpr const char * VCPU_DIR = "/sys/fs/cgroup/cpu/libvirt/qemu/";
};

struct VmId {
    VmId(): 
        start_timestamp(0), pid(0) {};
    VmId(std::time_t start_timestamp, pid_t pid): 
        start_timestamp(start_timestamp), pid(pid) {}
    std::time_t start_timestamp;
    pid_t pid;
};

bool operator==(const VmId &lv, const VmId &rv);
bool operator<(const VmId &lv, const VmId &rv);
std::ostream &operator<<(std::ostream &os, const VmId &v);

#endif
