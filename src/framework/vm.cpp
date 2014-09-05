#include "framework/vm.h"
#include <map>
#include "utils/log.h"

using namespace std;

VmSet::VmSet()
{
    func_option_ = FuncOption::get_instance();
    vm_base_ = VmBase::get_instance();
    vm_cpu_usage_ = VmCpuUsage::get_instance();
    vm_cache_miss_ = VmCacheMiss::get_instance();
}

VmSet::~VmSet()
{
    if (func_option_->check_option(Option::OP_VM_BASE))
    {
        map<OptionParam, OptionParamVal> &param = func_option_->get_param(Option::OP_VM_BASE);
        for (auto & p : param) 
        {
            switch(p.first)
            {
            case OptionParam::CALLBACK:
                delete (vm_base_callback_t*)p.second.get_pointer();
                break;
            default:
                break;
            }
        }
    }
    if (func_option_->check_option(Option::OP_VM_CPU_USAGE)) {
        map<OptionParam, OptionParamVal> &param = func_option_->get_param(Option::OP_VM_CPU_USAGE);
        for (auto & p : param) 
        {
            switch(p.first)
            {
            case OptionParam::CALLBACK:
                delete (cpu_usage_callback_t*)p.second.get_pointer();
                break;
            default:
                break;
            }
        }
    }
    if (func_option_->check_option(Option::OP_VM_CACHE_MISS)) {
        map<OptionParam, OptionParamVal> &param = func_option_->get_param(Option::OP_VM_CACHE_MISS);
        for (auto & p : param) 
        {
            switch(p.first)
            {
            case OptionParam::CALLBACK:
                delete (cache_miss_callback_t*)p.second.get_pointer();
                break;
            default:
                break;
            }
        }
    }
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
    lock_guard<mutex> lock(init_vms_mutex_);
    //TODO check whether host is inited
    host = host;

    set<VM> vms;

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
            case OptionParam::LOOP_INTERVAL:
                vm_base_->set_interval(p.second.get_int());
                break;
            case OptionParam::CALLBACK:
                vm_base_->set_callback(*((vm_base_callback_t*)p.second.get_pointer()));
                break;
            default:
                //cerr message "invalid parameter for OP_VM_BASE"
                break;
            }
        }
        if (!vm_base_->joinable())
            vm_base_->start();
    }
    if (func_option_->check_option(Option::OP_VM_CPU_USAGE)) {
        map<OptionParam, OptionParamVal> &param = func_option_->get_param(Option::OP_VM_CPU_USAGE);
        for (auto & p : param) 
        {
            switch(p.first)
            {
            case OptionParam::LOOP_INTERVAL:
                vm_cpu_usage_->set_interval(p.second.get_int());
                break;
            case OptionParam::CALLBACK:
                vm_cpu_usage_->set_callback(*((cpu_usage_callback_t*)p.second.get_pointer()));
                break;
            default:
                //cerr message "invalid parameter"
                break;
            }
        }
        if (!vm_cpu_usage_->joinable())
            vm_cpu_usage_->start();
    }
    if (func_option_->check_option(Option::OP_VM_CACHE_MISS)) {
        map<OptionParam, OptionParamVal> &param = func_option_->get_param(Option::OP_VM_CACHE_MISS);
        for (auto & p : param) 
        {
            switch(p.first)
            {
            case OptionParam::LOOP_INTERVAL:
                vm_cache_miss_->set_loop_interval(p.second.get_int());
                break;
            case OptionParam::SAMPLE_INTERVAL:
                vm_cache_miss_->set_sample_interval(p.second.get_int());
                break;
            case OptionParam::CALLBACK:
                vm_cache_miss_->set_callback(*((cache_miss_callback_t*)p.second.get_pointer()));
                break;
            default:
                //cerr message "invalid parameter"
                break;
            }
        }
        if (!vm_cache_miss_->joinable())
            vm_cache_miss_->start();
    }

    set<VmId> vm_ids;
    if (vm_cmd == "")
        vm_ids = vm_base_->get_vm_ids();
    else
        vm_ids = vm_base_->get_vm_ids(vm_cmd);
    for (auto& vm_id : vm_ids)
        vms.insert(VM(vm_id));

    return vms;
}

VM::VM(VmId vm_id)
{
    vm_base_ = VmBase::get_instance();
    vm_cpu_usage_ = VmCpuUsage::get_instance();
    vm_cache_miss_ = VmCacheMiss::get_instance();
    cpu_mig_ = CpuMig::get_instance();
    vm_id_ = vm_id;
}

//OP_VM_BASE
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

//OP_VM_CPU_USAGE
int VM::sys_cpu_usage() const 
{
    return vm_cpu_usage_->get_sys_cpu_usage();
}

int VM::cpu_usage() const
{
    return vm_cpu_usage_->get_cpu_usage(vm_id_);
}

int VM::cpu_usage(pid_t vmthread_id) const
{
    return vm_cpu_usage_->get_cpu_usage(vmthread_id);
}

HpthreadId VM::running_on_hpthread(pid_t vmthread_id) const
{
    return vm_cpu_usage_->get_running_on_hpthread(vmthread_id);
}

//OP_VM_CACHE_MISS
CacheMissData VM::cache_miss() const
{
    return vm_cache_miss_->get_cache_miss(vm_id_);
}

CacheMissData VM::cache_miss(pid_t vmthread_id) const
{
    return vm_cache_miss_->get_cache_miss(vmthread_id);
}

//VCPU MIG
void VM::set_vcpu_mig(pid_t vmthread_id, std::set<HpthreadId> hpthread_ids) const
{
    set<int> ids; 
    for (auto& id : hpthread_ids)
        ids.insert(id.id);
    cpu_mig_->set_cpu_mig(vmthread_id, ids);
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
