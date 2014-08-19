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

    has_data = false;
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
    has_data = false;

    unique_lock<shared_timed_mutex> vm_ids_lock(vm_ids_.mutex_, defer_lock);
    unique_lock<shared_timed_mutex> name_lock(name_.mutex_, defer_lock);
    unique_lock<shared_timed_mutex> uuid_lock(uuid_.mutex_, defer_lock);
    unique_lock<shared_timed_mutex> vsocket_num_lock(vsocket_num_.mutex_, defer_lock);
    unique_lock<shared_timed_mutex> vcore_num_lock(vcore_num_.mutex_, defer_lock);
    unique_lock<shared_timed_mutex> vhpthread_num_lock(vhpthread_num_.mutex_, defer_lock);
    unique_lock<shared_timed_mutex> total_mem_size_lock(total_mem_size_.mutex_, defer_lock);
    unique_lock<shared_timed_mutex> vcpu_ids_lock(vcpu_ids_.mutex_, defer_lock);
    unique_lock<shared_timed_mutex> stable_vmthread_ids_lock(stable_vmthread_ids_.mutex_, defer_lock);
    lock(
        vm_ids_lock, 
        name_lock,
        uuid_lock,
        vsocket_num_lock,
        vcore_num_lock,
        vhpthread_num_lock,
        total_mem_size_lock,
        vcpu_ids_lock,
        stable_vmthread_ids_lock
        );
    vm_ids_.value_.clear();
    name_.value_.clear();
    uuid_.value_.clear();
    vsocket_num_.value_.clear();
    vcore_num_.value_.clear();
    vhpthread_num_.value_.clear();
    total_mem_size_.value_.clear();
    vcpu_ids_.value_.clear();
    stable_vmthread_ids_.value_.clear();
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
        refresh();
    while(!has_data) 
        this_thread::sleep_for(chrono::milliseconds(10));
    return vm_ids_.read();
}

string VmBase::get_name(VmId vm_id)
{
    if(!joinable())
        refresh();
    while(!has_data) 
        this_thread::sleep_for(chrono::milliseconds(10));
    return name_.find(vm_id, {vm_id, ""}).second;
}

string VmBase::get_uuid(VmId vm_id)
{
    if(!joinable())
        refresh();
    while(!has_data) 
        this_thread::sleep_for(chrono::milliseconds(10));
    return uuid_.find(vm_id, {vm_id, ""}).second;
}

int VmBase::get_vsocket_num(VmId vm_id)
{
    if(!joinable())
        refresh();
    while(!has_data) 
        this_thread::sleep_for(chrono::milliseconds(10));
    return vsocket_num_.find(vm_id, {vm_id, -1}).second;
}

int VmBase::get_vcore_num(VmId vm_id)
{
    if(!joinable())
        refresh();
    while(!has_data) 
        this_thread::sleep_for(chrono::milliseconds(10));
    return vcore_num_.find(vm_id, {vm_id, -1}).second;
}

int VmBase::get_vhpthread_num(VmId vm_id)
{
    if(!joinable())
        refresh();
    while(!has_data) 
        this_thread::sleep_for(chrono::milliseconds(10));
    return vhpthread_num_.find(vm_id, {vm_id, -1}).second;
}

int VmBase::get_total_mem_size(VmId vm_id)
{
    if(!joinable())
        refresh();
    while(!has_data) 
        this_thread::sleep_for(chrono::milliseconds(10));
    return total_mem_size_.find(vm_id, {vm_id, -1}).second;
}

set<pid_t> VmBase::get_vcpu_ids(VmId vm_id)
{
    if(!joinable())
        refresh();
    while(!has_data) 
        this_thread::sleep_for(chrono::milliseconds(10));
    return vcpu_ids_.find(vm_id, {vm_id, {}}).second;
}

int VmBase::get_vcpu_num(VmId vm_id)
{
    if(!joinable())
        refresh();
    while(!has_data) 
        this_thread::sleep_for(chrono::milliseconds(10));
    return vcpu_ids_.find(vm_id, {vm_id, {}}).second.size();
}

set<pid_t> VmBase::get_stable_vmthread_ids(VmId vm_id)
{
    if(!joinable())
        refresh();
    while(!has_data) 
        this_thread::sleep_for(chrono::milliseconds(10));
    return stable_vmthread_ids_.find(vm_id, {vm_id, {}}).second;
}

int VmBase::get_stable_vmthread_num(VmId vm_id)
{
    if(!joinable())
        refresh();
    while(!has_data) 
        this_thread::sleep_for(chrono::milliseconds(10));
    return stable_vmthread_ids_.find(vm_id, {vm_id, {}}).second.size();
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

    //refresh most
    set<VmId> vm_ids;
    map<VmId, string> names;
    map<VmId, string> uuids;
    map<VmId, int> vsocket_nums;
    map<VmId, int> vcore_nums;;
    map<VmId, int> vhpthread_nums;;
    map<VmId, int> total_mem_sizes;;

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
        vm_ids.insert(vm_id);

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
        names[vm_id] = name;

        //add uuid
        string uuid = "";
        auto uuid_iter = find(args.begin(), args.end(), "-uuid");
        if (uuid_iter != args.end())
        {
            ++uuid_iter;
            uuid = *uuid_iter;
        }
        uuids[vm_id] = uuid;

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
                        vsocket_nums[vm_id] = stoi(data[1]);
                    else if(data[0] == "cores")
                        vcore_nums[vm_id] = stoi(data[1]);
                    else if(data[0] == "threads")
                        vhpthread_nums[vm_id] = stoi(data[1]);
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
        total_mem_sizes[vm_id] = size;
    }
    pclose(data);

    //refresh_vcpu
    map<VmId, set<pid_t> > vcpu_ids;
    map<VmId, set<pid_t> > stable_vmthread_ids;
    for (auto& vm_id : vm_ids) {
        set<pid_t> ids;
        vector<string> vcpu_dirs;
        str_tools::get_dirs(VCPU_DIR + names[vm_id] + "/", "vcpu", &vcpu_dirs);
        for (size_t i = 0; i < vcpu_dirs.size(); ++i) {
            pid_t pid;
            ifstream fin(VCPU_DIR + names[vm_id] + "/" + vcpu_dirs[i] + "/tasks");
            if (!fin.good()) {
                //TODO throw
    //            LOG(LogLevel::err) 
                return;
            }
            fin >> pid;
            ids.insert(pid);
        }
        vcpu_ids[vm_id] = ids;
        stable_vmthread_ids[vm_id] = ids;
        stable_vmthread_ids[vm_id].insert(vm_id.pid);
    }
    
    //write_data
    unique_lock<shared_timed_mutex> vm_ids_lock(vm_ids_.mutex_, defer_lock);
    unique_lock<shared_timed_mutex> name_lock(name_.mutex_, defer_lock);
    unique_lock<shared_timed_mutex> uuid_lock(uuid_.mutex_, defer_lock);
    unique_lock<shared_timed_mutex> vsocket_num_lock(vsocket_num_.mutex_, defer_lock);
    unique_lock<shared_timed_mutex> vcore_num_lock(vcore_num_.mutex_, defer_lock);
    unique_lock<shared_timed_mutex> vhpthread_num_lock(vhpthread_num_.mutex_, defer_lock);
    unique_lock<shared_timed_mutex> total_mem_size_lock(total_mem_size_.mutex_, defer_lock);
    unique_lock<shared_timed_mutex> vcpu_ids_lock(vcpu_ids_.mutex_, defer_lock);
    unique_lock<shared_timed_mutex> stable_vmthread_ids_lock(stable_vmthread_ids_.mutex_, defer_lock);
    lock(
        vm_ids_lock, 
        name_lock,
        uuid_lock,
        vsocket_num_lock,
        vcore_num_lock,
        vhpthread_num_lock,
        total_mem_size_lock,
        vcpu_ids_lock,
        stable_vmthread_ids_lock
        );

    vm_ids_.value_ = vm_ids;
    name_.value_ = names;
    uuid_.value_ = uuids;
    vsocket_num_.value_ = vsocket_nums;
    vcore_num_.value_ = vcore_nums;
    vhpthread_num_.value_ = vhpthread_nums;
    total_mem_size_.value_ = total_mem_sizes;
    vcpu_ids_.value_ = vcpu_ids;
    stable_vmthread_ids_.value_ = stable_vmthread_ids;
    has_data = true;
    return;

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
    set<pid_t> stable_vmthread_ids = stable_vmthread_ids_.find(vm_id, {vm_id, {}}).second;
    if (stable_vmthread_ids.size() == 0)
    {
        //this VM is down.
        return {};
    }

    set_difference(vmthread_ids.begin(), vmthread_ids.end(), stable_vmthread_ids.begin(), stable_vmthread_ids.end(),
        inserter(volatile_vmthread_ids, volatile_vmthread_ids.end()));
    return volatile_vmthread_ids;
}

void VmBase::run()
{
    while(!stop_)
    {
        refresh();
        this_thread::sleep_for(chrono::seconds(5));
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
