#include "sysinfo/vm/dynamic/vm_base.h"

#include <cstdio>
#include <cmath>
#include <regex>
#include <fstream>
#include <algorithm>
#include <iterator>
#include "utils/str_tools.h"
#include "utils/log.h"
#include "framework/exception.h"

using namespace std;

VmBase::VmBase(): has_data_(false), interval_ms_(5000)
{
    buf_ = new char[BUF_SIZE];
    callback_func_ = new vm_base_callback_t;
    *callback_func_ = nullptr;
}

VmBase::~VmBase()
{
    stop();
    delete[] buf_;
    delete callback_func_;
}

VmBase *VmBase::get_instance()
{
    static VmBase vm_base;
    return &vm_base;
}

void VmBase::set_vm_cmd(std::string vm_cmd)
{
    unique_lock<shared_timed_mutex> lock(data_mutex_);

    has_data_ = false;
    vm_ids_.clear();
    stable_vmthread_id_to_vm_id_.clear();
    name_.clear();
    uuid_.clear();
    vsocket_num_.clear();
    vcore_num_.clear();
    vhpthread_num_.clear();
    total_mem_size_.clear();
    vcpu_ids_.clear();
    stable_vmthread_ids_.clear();
    vm_cmd_ = vm_cmd;
}

void VmBase::set_interval(int interval_ms) 
{
    interval_ms_ = interval_ms;
}

void VmBase::set_callback(vm_base_callback_t callback_func) 
{
    *callback_func_ = callback_func;
}

std::set<VmId> VmBase::get_vm_ids(string vm_cmd)
{
    set_vm_cmd(vm_cmd);
    return get_vm_ids();
}

std::set<VmId> VmBase::get_vm_ids()
{
    if(!joinable())
        refresh();
    while(!has_data_) 
        this_thread::sleep_for(chrono::milliseconds(10));
    std::shared_lock<std::shared_timed_mutex> lock(data_mutex_);
    return vm_ids_;
}

VmId VmBase::stable_vmthread_id_to_vm_id(pid_t pid)
{
    if(!joinable())
        refresh();
    while(!has_data_) 
        this_thread::sleep_for(chrono::milliseconds(10));
    std::shared_lock<std::shared_timed_mutex> lock(data_mutex_);
    auto iter = stable_vmthread_id_to_vm_id_.find(pid);
    if (iter != stable_vmthread_id_to_vm_id_.end())
        return iter->second;
    else
        return {};
}

template <typename T>
T VmBase::get_data_by_vm_id(std::map<VmId, T>& data, VmId vm_id, const T& failed_ret) {
    if(!joinable())
        refresh();
    while(!has_data_) 
        this_thread::sleep_for(chrono::milliseconds(10));
    std::shared_lock<std::shared_timed_mutex> lock(data_mutex_);
    if (data.find(vm_id) != data.end())
        return data[vm_id];
    else
        return failed_ret;
}

string VmBase::get_name(VmId vm_id)
{
    return get_data_by_vm_id(name_, vm_id, {});
}

string VmBase::get_uuid(VmId vm_id)
{
    return get_data_by_vm_id(uuid_, vm_id, {});
}

int VmBase::get_vsocket_num(VmId vm_id)
{
    return get_data_by_vm_id(vsocket_num_, vm_id, -1);
}

int VmBase::get_vcore_num(VmId vm_id)
{
    return get_data_by_vm_id(vcore_num_, vm_id, -1);
}

int VmBase::get_vhpthread_num(VmId vm_id)
{
    return get_data_by_vm_id(vhpthread_num_, vm_id, -1);
}

int VmBase::get_total_mem_size(VmId vm_id)
{
    return get_data_by_vm_id(total_mem_size_, vm_id, -1);
}

set<pid_t> VmBase::get_vcpu_ids(VmId vm_id)
{
    return get_data_by_vm_id(vcpu_ids_, vm_id, {});
}

int VmBase::get_vcpu_num(VmId vm_id)
{
    return get_data_by_vm_id(vcpu_ids_, vm_id, {}).size();
}

set<pid_t> VmBase::get_stable_vmthread_ids(VmId vm_id)
{
    return get_data_by_vm_id(stable_vmthread_ids_, vm_id, {});
}

int VmBase::get_stable_vmthread_num(VmId vm_id)
{
    return get_data_by_vm_id(stable_vmthread_ids_, vm_id, {}).size();
}

set<pid_t> VmBase::get_volatile_vmthread_ids(VmId vm_id)
{
    return refresh_volatile_vmthread(vm_id);
}

int VmBase::get_volatile_vmthread_num(VmId vm_id)
{
    return refresh_volatile_vmthread(vm_id).size();
}

void VmBase::refresh()
{
    unique_lock<shared_timed_mutex> lock(data_mutex_);

    refresh_most();
    refresh_vcpu_stable_vmthread();

    for (auto& vm_id : vm_ids_) {
        if (*callback_func_) {
            (*callback_func_)(
                vm_id,
                name_[vm_id],
                uuid_[vm_id],
                vsocket_num_[vm_id],
                vcore_num_[vm_id],
                vhpthread_num_[vm_id],
                total_mem_size_[vm_id],
                vcpu_ids_[vm_id],
                stable_vmthread_ids_[vm_id]
                );
        }
    }

//    LDEBUG << "DDD----" << endl;
//    for (auto& vm_id : vm_ids_) {
//        LDEBUG << "DDD " << vm_id << ">>";
//        for (auto& id : vcpu_ids_[vm_id]) {
//            LDEBUG << ":" << id;
//        }
//        LDEBUG << endl;
//    }
    has_data_ = true;
    return;

}

void VmBase::refresh_most()
{
    vm_ids_.clear();
    stable_vmthread_id_to_vm_id_.clear();
    name_.clear();
    uuid_.clear();
    vsocket_num_.clear();
    vcore_num_.clear();
    vhpthread_num_.clear();
    total_mem_size_.clear();

    string cmd = "ps -C " + vm_cmd_ + " -wwo etime=,pid=,args=";
    time_t cur_time;
    time(&cur_time);
    FILE* data = popen(cmd.c_str(), "r");

    string pid;
    string start_timestamp;
    vector<string> args;
    string tmp_str;
    while(fgets(buf_, BUF_SIZE, data))
    {
        args.clear();
        istringstream is(buf_);
        is >> start_timestamp;

        long long days = 0;
        long long  hours = 0;
        long long  minutes = 0;
        long long  seconds = 0;
        time_t start_time;
        if (count(start_timestamp.begin(), start_timestamp.end(), '-') > 0)
            sscanf(start_timestamp.c_str(), "%lld-%lld:%lld:%lld", &days, &hours, &minutes, &seconds);
        else if (count(start_timestamp.begin(), start_timestamp.end(), ':') == 2)
            sscanf(start_timestamp.c_str(), "%lld:%lld:%lld", &hours, &minutes, &seconds);
        else if (count(start_timestamp.begin(), start_timestamp.end(), ':') == 1)
            sscanf(start_timestamp.c_str(), "%lld:%lld", &minutes, &seconds);
        start_time = cur_time - (((days * 24 + hours) * 60 + minutes) * 60 + seconds);

        is >> pid;
        while(is)
        {
            is >> tmp_str;
            args.push_back(tmp_str);
        }

        //add vm_ids
        VmId vm_id(start_time, stoull(pid));
        vm_ids_.insert(vm_id);

        //add name
        string name = "";
        auto name_iter = find(args.begin(), args.end(), "-name");
        if (name_iter != args.end())
        {
            ++name_iter;
            name = *name_iter;
        }
        else
        {
            //TODO throw //because of cgroup need it
        }
        name_[vm_id] = name;

        //add uuid
        string uuid = "";
        auto uuid_iter = find(args.begin(), args.end(), "-uuid");
        if (uuid_iter != args.end())
        {
            ++uuid_iter;
            uuid = *uuid_iter;
        }
        uuid_[vm_id] = uuid;

        //add vsocket vcore vhpthread
        auto iter = find(args.begin(), args.end(), "-smp");
        if (iter != args.end())
        {
            ++iter;
            vector<string> ops;
            str_tools::split(*iter, ',', ops);
            for(auto& op : ops)
            {
                vector<string> data;
                str_tools::split(op, '=', data);
                if (data.size() == 2) {
                    if(data[0] == "sockets")
                        vsocket_num_[vm_id] = stoi(data[1]);
                    else if(data[0] == "cores")
                        vcore_num_[vm_id] = stoi(data[1]);
                    else if(data[0] == "threads")
                        vhpthread_num_[vm_id] = stoi(data[1]);
                }
            }
        }

        //add total_mem_size
        int size = -1;
        auto size_iter = find(args.begin(), args.end(), "-m");
        if (size_iter != args.end())
        {
            ++size_iter;
            size = stoi(*size_iter);
        }
        total_mem_size_[vm_id] = size;
    }
    pclose(data);
}

void VmBase::refresh_vcpu_stable_vmthread()
{
    vcpu_ids_.clear();
    stable_vmthread_ids_.clear();

    for (auto& vm_id : vm_ids_) {
        set<pid_t> ids;
        vector<string> vcpu_dirs;
        str_tools::get_dirs(VCPU_DIR + name_[vm_id] + "/", "vcpu", &vcpu_dirs);
        for (size_t i = 0; i < vcpu_dirs.size(); ++i) {
            pid_t pid;
            string file_path = VCPU_DIR + name_[vm_id] + "/" + vcpu_dirs[i] + "/tasks";
            ifstream fin(file_path);
            if (!fin.good()) {
                THROW(FileOpenFailed, "To open file: " + file_path);
                return;
            }
            fin >> pid;
            ids.insert(pid);
        }
        vcpu_ids_[vm_id] = ids;
        stable_vmthread_ids_[vm_id] = ids;
        stable_vmthread_ids_[vm_id].insert(vm_id.pid);
        for (auto& id : stable_vmthread_ids_[vm_id])
            stable_vmthread_id_to_vm_id_[id] = vm_id;
    }
}

set<pid_t> VmBase::refresh_volatile_vmthread(VmId vm_id)
{
    set<pid_t> vmthread_ids;
    vector<string> vmthread_dirs;
    str_tools::get_dirs("/proc/" + to_string(vm_id.pid) + "/task/", "", &vmthread_dirs);
    for (auto& dir : vmthread_dirs) {
        vmthread_ids.insert(stoi(dir));
    }

    set<pid_t> volatile_vmthread_ids;
    set<pid_t> stable_vmthread_ids = get_stable_vmthread_ids(vm_id);
    if (stable_vmthread_ids.size() == 0)
    {
        //this VM is down.
        return {};
    }

    set_difference(vmthread_ids.begin(), vmthread_ids.end(), stable_vmthread_ids.begin(), stable_vmthread_ids.end(),
        inserter(volatile_vmthread_ids, volatile_vmthread_ids.end()));
    return volatile_vmthread_ids;
}

void VmBase::clear()
{
    unique_lock<shared_timed_mutex> lock(data_mutex_);
    vm_ids_.clear();
    stable_vmthread_id_to_vm_id_.clear();
    name_.clear();
    uuid_.clear();
    vsocket_num_.clear();
    vcore_num_.clear();
    vhpthread_num_.clear();
    total_mem_size_.clear();
    vcpu_ids_.clear();
    stable_vmthread_ids_.clear();

    has_data_ = false;
}

void VmBase::run()
{
    while(!stop_)
    {
        refresh();
        this_thread::sleep_for(chrono::milliseconds(interval_ms_));
    }
    clear();
}

bool operator==(const VmId &lv, const VmId &rv)
{
    return abs(difftime(lv.start_timestamp, rv.start_timestamp)) <= 2
        && lv.pid == rv.pid;
}

bool operator<(const VmId &lv, const VmId &rv)
{
    if (abs(difftime(lv.start_timestamp, rv.start_timestamp)) <= 2)
        return lv.pid < rv.pid;
    else
        return difftime(lv.start_timestamp, rv.start_timestamp) > 2;
}

ostream &operator<<(ostream &os, const VmId &v)
{
    os << v.start_timestamp << ":" << v.pid;
    return os;
}
