#include "framework/vm.h"
#include <map>

using namespace std;

VmSet::VmSet()
{
    func_option_ = FuncOption::get_instance();
    vm_base_ = VmBase::get_instance();
}

VmSet* VmSet::get_instance()
{
    static VmSet vm_set;
    return &vm_set;
}

std::set<VM> VmSet::init_vms(Host *host)
{
    return init_vms(host, "");
}

std::set<VM> VmSet::init_vms(Host *host, string vm_cmd)
{
    //TODO check whether host is inited
    host = host;

    vms_.clear();

    if (func_option_->check_option(Option::OP_VM_BASE))
    {
        map<OptionParam, OptionParamVal> &param = func_option_->get_param(Option::OP_VM_BASE);
        for (auto & p : param) 
        {
            switch(p.first)
            {
            case OptionParam::VM_CMD:
                vm_base_->set_vm_cmd(p.second.get_string());
                break;
            default:
                //cerr message "invalid parameter for OP_VM_BASE"
                break;
            }
        }
        if (!vm_base_->joinable())
            vm_base_->start();
    }

    set<VmId> vm_ids;
    if (vm_cmd == "")
        vm_ids = vm_base_->get_vm_ids();
    else
        vm_ids = vm_base_->get_vm_ids(vm_cmd);
    for (auto& vm_id : vm_ids)
        vms_.insert(VM(vm_id));

    return vms_;
}

VM::VM(VmId vm_id)
{
    vm_base_ = VmBase::get_instance();
    vm_id_ = vm_id;
}

VmId VM::vm_id() const
{
    return vm_id_;
}

string VM::name() const
{
    return vm_base_->get_name(vm_id_);
}

string VM::uuid() const
{
    return vm_base_->get_uuid(vm_id_);
}

int VM::vsocket_num() const
{
    return vm_base_->get_vsocket_num(vm_id_);
}

int VM::vcore_num() const
{
    return vm_base_->get_vcore_num(vm_id_);
}

int VM::vhpthread_num() const
{
    return vm_base_->get_vhpthread_num(vm_id_);
}

int VM::total_mem_size() const
{
    return vm_base_->get_total_mem_size(vm_id_);
}

set<pid_t> VM::vcpu_ids() const
{
    return vm_base_->get_vcpu_ids(vm_id_);
}

int VM::vcpu_num() const
{
    return vm_base_->get_vcpu_num(vm_id_);
}

set<pid_t> VM::stable_vmthread_ids() const
{
    return vm_base_->get_stable_vmthread_ids(vm_id_);
}

int VM::stable_vmthread_num() const
{
    return vm_base_->get_stable_vmthread_num(vm_id_);
}

set<pid_t> VM::volatile_vmthread_ids() const
{
    return vm_base_->get_volatile_vmthread_ids(vm_id_);
}

int VM::volatile_vmthread_num() const
{
    return vm_base_->get_volatile_vmthread_num(vm_id_);
}

VM& VM::operator=(const VM &v)
{
    vm_id_ = v.vm_id();
    return *this;
}

bool operator<(const VM &lv, const VM &rv)
{
    return lv.vm_id() < rv.vm_id();
}
