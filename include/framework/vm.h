#ifndef _VM_
#define _VM_

#include <set>
#include <string>
#include "utils/func_option.h"
#include "sysinfo/vm/dynamic/vm_base.h"

class Host;
class VM;

class VmSet {
public:
    static VmSet* get_instance();
    std::set<VM> init_vms(Host *host);
    std::set<VM> init_vms(Host *host, std::string vm_cmd);
private:
    VmSet();
    std::set<VM> vms_;

    FuncOption *func_option_;
    VmBase *vm_base_;
};

class VM {
public:
    VM(VmId vm_id);
    VM &operator=(const VM &v);
    
    VmId vm_id() const;
    std::string name() const;
    std::string uuid() const;
    int vsocket_num() const;
    int vcore_num() const;
    int vhpthread_num() const;
    int total_mem_size() const;
private:
    VmId vm_id_;

    VmBase *vm_base_;
};

bool operator<(const VM &lv, const VM &rv);

#endif
