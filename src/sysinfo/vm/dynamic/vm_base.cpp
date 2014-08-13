#include "sysinfo/vm/dynamic/vm_base.h"

#include <cstdio>
#include <cmath>
#include <iostream>
#include <regex>
#include <fstream>
#include <algorithm>
#include <iterator>
#include "utils/str_tools.h"

using namespace std;

VmBase::VmBase()
{
//    FILE* fp = fopen(PID_MAX_FILE, "r");
//    if (fp == nullptr) {
//        //TODO throw except
////        LOG(LogLevel::err) 
////            << "VmProcs::get_pid_max: " << strerror(errno) << endl;
//    } else {
//        fscanf(fp, "%llu", &pid_max_);
//    }

    has_data_most = false;
    has_data_vcpu = false;
    buf_ = new char[BUF_SIZE];
}

VmBase::~VmBase()
{
    delete[] buf_;
    stop();
}

VmBase *VmBase::get_instance()
{
    static VmBase vm_base;
    return &vm_base;
}

void VmBase::set_vm_cmd(std::string vm_cmd)
{
    lock_guard<mutex> ids_lock(vm_ids_mutex_);
    lock_guard<mutex> name_lock(name_mutex_);
    lock_guard<mutex> uuid_lock(uuid_mutex_);
    lock_guard<mutex> vsocket_num_lock(vsocket_num_mutex_);
    lock_guard<mutex> vcore_num_lock(vcore_num_mutex_);
    lock_guard<mutex> vhpthread_num_lock(vhpthread_num_mutex_);
    lock_guard<mutex> total_mem_size_lock(total_mem_size_mutex_);
    lock_guard<mutex> vcpu_ids_lock(vcpu_ids_mutex_);
    lock_guard<recursive_mutex> stable_vmthread_ids_lock(stable_vmthread_ids_mutex_);
    has_data_most = false;
    has_data_vcpu = false;

    vm_ids_.clear();
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

std::set<VmId> VmBase::get_vm_ids(string vm_cmd)
{
    set_vm_cmd(vm_cmd);
    return get_vm_ids();
}

std::set<VmId> VmBase::get_vm_ids()
{
    if(!joinable())
        refresh_most();
    while(!has_data_most) 
        this_thread::sleep_for(chrono::milliseconds(10));
    lock_guard<mutex> ids_lock(vm_ids_mutex_);
    return vm_ids_;
}

string VmBase::get_name(VmId vm_id)
{
    if(!joinable())
        refresh_most();
    while(!has_data_most) 
        this_thread::sleep_for(chrono::milliseconds(10));
    lock_guard<mutex> name_lock(name_mutex_);
    if (name_.find(vm_id) != name_.end())
        return name_[vm_id];
    else
        return "";
}

string VmBase::get_uuid(VmId vm_id)
{
    if(!joinable())
        refresh_most();
    while(!has_data_most) 
        this_thread::sleep_for(chrono::milliseconds(10));
    lock_guard<mutex> uuid_lock(uuid_mutex_);
    if (uuid_.find(vm_id) != uuid_.end())
        return uuid_[vm_id];
    else
        return "";
}

int VmBase::get_vsocket_num(VmId vm_id)
{
    if(!joinable())
        refresh_most();
    while(!has_data_most) 
        this_thread::sleep_for(chrono::milliseconds(10));
    lock_guard<mutex> vsocket_num_lock(vsocket_num_mutex_);
    if (vsocket_num_.find(vm_id) != vsocket_num_.end())
        return vsocket_num_[vm_id];
    else
        return -1;
}

int VmBase::get_vcore_num(VmId vm_id)
{
    if(!joinable())
        refresh_most();
    while(!has_data_most) 
        this_thread::sleep_for(chrono::milliseconds(10));
    lock_guard<mutex> vcore_num_lock(vcore_num_mutex_);
    if (vcore_num_.find(vm_id) != vcore_num_.end())
        return vcore_num_[vm_id];
    else
        return -1;
}

int VmBase::get_vhpthread_num(VmId vm_id)
{
    if(!joinable())
        refresh_most();
    while(!has_data_most) 
        this_thread::sleep_for(chrono::milliseconds(10));
    lock_guard<mutex> vhpthread_num_lock(vhpthread_num_mutex_);
    if (vhpthread_num_.find(vm_id) != vhpthread_num_.end())
        return vhpthread_num_[vm_id];
    else
        return -1;
}

int VmBase::get_total_mem_size(VmId vm_id)
{
    if(!joinable())
        refresh_most();
    while(!has_data_most) 
        this_thread::sleep_for(chrono::milliseconds(10));
    lock_guard<mutex> total_mem_size_lock(total_mem_size_mutex_);
    if (total_mem_size_.find(vm_id) != total_mem_size_.end())
        return total_mem_size_[vm_id];
    else
        return -1;
}

set<pid_t> VmBase::get_vcpu_ids(VmId vm_id)
{
    if(!joinable())
        refresh_vcpu();
    while(!has_data_vcpu) 
        this_thread::sleep_for(chrono::milliseconds(10));
    lock_guard<mutex> vcpu_ids_lock(vcpu_ids_mutex_);
    if (vcpu_ids_.find(vm_id) != vcpu_ids_.end())
        return vcpu_ids_[vm_id];
    else
        return {};
}

int VmBase::get_vcpu_num(VmId vm_id)
{
    if(!joinable())
        refresh_vcpu();
    while(!has_data_vcpu) 
        this_thread::sleep_for(chrono::milliseconds(10));
    lock_guard<mutex> vcpu_ids_lock(vcpu_ids_mutex_);
    if (vcpu_ids_.find(vm_id) != vcpu_ids_.end())
        return vcpu_ids_[vm_id].size();
    else
        return -1;
}

set<pid_t> VmBase::get_stable_vmthread_ids(VmId vm_id)
{
    if(!joinable())
        refresh_vcpu();
    while(!has_data_vcpu) 
        this_thread::sleep_for(chrono::milliseconds(10));
    lock_guard<recursive_mutex> stable_vmthread_ids_lock(stable_vmthread_ids_mutex_);
    if (stable_vmthread_ids_.find(vm_id) != stable_vmthread_ids_.end())
        return stable_vmthread_ids_[vm_id];
    else
        return {};
}

int VmBase::get_stable_vmthread_num(VmId vm_id)
{
    if(!joinable())
        refresh_vcpu();
    while(!has_data_vcpu) 
        this_thread::sleep_for(chrono::milliseconds(10));
    lock_guard<recursive_mutex> stable_vmthread_ids_lock(stable_vmthread_ids_mutex_);
    if (stable_vmthread_ids_.find(vm_id) != stable_vmthread_ids_.end())
        return stable_vmthread_ids_[vm_id].size();
    else
        return -1;
}

set<pid_t> VmBase::get_volatile_vmthread_ids(VmId vm_id)
{
    return refresh_volatile_vmthread(vm_id);
}

int VmBase::get_volatile_vmthread_num(VmId vm_id)
{
    return refresh_volatile_vmthread(vm_id).size();
}

void VmBase::refresh_most()
{
    lock_guard<mutex> vm_ids_lock(vm_ids_mutex_);
    lock_guard<mutex> name_lock(name_mutex_);
    lock_guard<mutex> uuid_lock(uuid_mutex_);
    lock_guard<mutex> vsocket_num_lock(vsocket_num_mutex_);
    lock_guard<mutex> vcore_num_lock(vcore_num_mutex_);
    lock_guard<mutex> vhpthread_num_lock(vhpthread_num_mutex_);
    lock_guard<mutex> total_mem_size_lock(total_mem_size_mutex_);
    has_data_most = true;

    vm_ids_.clear();
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
        istringstream is(buf_);
        is >> start_timestamp;

        int days;
        int hours;
        int minutes;
        int seconds;
        sscanf(start_timestamp.c_str(), "%d-%d:%d:%d", &days, &hours, &minutes, &seconds);
        cur_time -= ((days * 24 + hours) * 60 + minutes) * 60 + seconds;

        is >> pid;
        while(is)
        {
            is >> tmp_str;
            args.push_back(tmp_str);
        }

        //add vm_ids
        VmId vm_id(cur_time, stoull(pid));
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
    
    return;

}

void VmBase::refresh_vcpu()
{
    lock_guard<mutex> vm_ids_lock(vm_ids_mutex_);
    lock_guard<mutex> vcpu_lock(vcpu_ids_mutex_);
    lock_guard<recursive_mutex> stable_vmthread_ids_lock(stable_vmthread_ids_mutex_);
    has_data_vcpu = true;
    vcpu_ids_.clear();
    stable_vmthread_ids_.clear();

    for (auto& vm_id : vm_ids_) {
        set<pid_t> vcpu_ids;
        vector<string> vcpu_dirs;
        str_tools::get_dirs(VCPU_DIR + name_[vm_id] + "/", "vcpu", &vcpu_dirs);
        for (size_t i = 0; i < vcpu_dirs.size(); ++i) {
            pid_t pid;
            ifstream fin(VCPU_DIR + name_[vm_id] + "/" + vcpu_dirs[i] + "/tasks");
            if (!fin.good()) {
                //TODO throw
    //            LOG(LogLevel::err) 
    //                << "NodeCoreHpthread::refresh: " << strerror(errno) << endl;
                return;
            }
            fin >> pid;
            vcpu_ids.insert(pid);
        }
        vcpu_ids_[vm_id] = vcpu_ids;
        stable_vmthread_ids_[vm_id] = vcpu_ids;
        stable_vmthread_ids_[vm_id].insert(vm_id.pid);
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
    lock_guard<recursive_mutex> stable_vmthread_ids_lock(stable_vmthread_ids_mutex_);
    if (stable_vmthread_ids_.find(vm_id) == stable_vmthread_ids_.end())
    {
        refresh_vcpu();
        if (stable_vmthread_ids_.find(vm_id) == stable_vmthread_ids_.end())
            //this VM is down.
            return {};
    }

    set_difference(vmthread_ids.begin(), vmthread_ids.end(), stable_vmthread_ids_[vm_id].begin(), stable_vmthread_ids_[vm_id].end(),
        inserter(volatile_vmthread_ids, volatile_vmthread_ids.end()));
    return volatile_vmthread_ids;
}

void VmBase::run()
{
    while(!stop_)
    {
        refresh_most();
        refresh_vcpu();
        this_thread::sleep_for(chrono::seconds(1));
    }
}

bool operator==(const VmId &lv, const VmId &rv)
{
    return abs(difftime(lv.start_timestamp, rv.start_timestamp)) <= 2
        && lv.pid == rv.pid;
}

bool operator<(const VmId &lv, const VmId &rv)
{
    if(lv.start_timestamp == rv.start_timestamp)
        return lv.pid < rv.pid;
    else
        return lv.start_timestamp < rv.start_timestamp;
}

ostream &operator<<(ostream &os, const VmId &v)
{
    os << v.start_timestamp << ":" << v.pid;
    return os;
}
