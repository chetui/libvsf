#include "framework/vm.h"
#include <map>
#include <iostream>

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
    lock_guard<mutex> lock(init_vms_mutex_);
    //TODO check whether host is inited
    host = host;

    set<VM> vms;

    cout << "AAA" << endl;
    if (func_option_->check_option(Option::OP_VM_BASE))
    {
        cout << "BBB" << endl;
        map<OptionParam, OptionParamVal> &param = func_option_->get_param(Option::OP_VM_BASE);
        for (auto & p : param) 
        {
            switch(p.first)
            {
            case OptionParam::VM_CMD:
                vm_base_->set_vm_cmd(p.second.get_string());
                break;
            case OptionParam::INTERVAL:
                vm_base_->set_interval(p.second.get_int());
                break;
            default:
                //cerr message "invalid parameter for OP_VM_BASE"
                break;
            }
        }
        if (!vm_base_->joinable())
            vm_base_->start();
    }
    cout << "CCC" << endl;
    if (func_option_->check_option(Option::OP_VM_CPU_USAGE)) {
        cout << "DDD" << endl;
        map<OptionParam, OptionParamVal> &param = func_option_->get_param(Option::OP_VM_CPU_USAGE);
        cout << "DDD1" << endl;
        for (auto & p : param) 
        {
            cout << "DDD1" << endl;
            switch(p.first)
            {
            case OptionParam::INTERVAL:
                cout << "DDD2:" << p.second.get_int() << endl;
                vm_cpu_usage_->set_interval(p.second.get_int());
                cout << "DDD3" << endl;
                break;
            default:
                //cerr message "invalid parameter"
                break;
            }
            cout << "DDD4" << endl;
        }
        cout << "DDD5" << endl;
        if (!vm_cpu_usage_->joinable())
            vm_cpu_usage_->start();
        cout << "DDD6" << endl;
    }

    cout << "EEE" << endl;
    set<VmId> vm_ids;
    if (vm_cmd == "")
        vm_ids = vm_base_->get_vm_ids();
    else
        vm_ids = vm_base_->get_vm_ids(vm_cmd);
    for (auto& vm_id : vm_ids)
        vms.insert(VM(vm_id));
    cout << "FFF" << endl;

    return vms;
}

VM::VM(VmId vm_id)
{
    vm_base_ = VmBase::get_instance();
    vm_cpu_usage_ = VmCpuUsage::get_instance();
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

VM& VM::operator=(const VM &v)
{
    vm_id_ = v.vm_id();
    return *this;
}

bool operator<(const VM &lv, const VM &rv)
{
    return lv.vm_id() < rv.vm_id();
}
