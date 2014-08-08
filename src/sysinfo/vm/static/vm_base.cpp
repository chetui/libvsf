#include "sysinfo/vm/static/vm_base.h"

#include <cstdio>
#include <iostream>
#include <regex>

using namespace std;

VmBase::VmBase()
{
    FILE* fp = fopen(PID_MAX_FILE, "r");
    if (fp == nullptr) {
        //TODO throw except
//        LOG(LogLevel::err) 
//            << "VmProcs::get_pid_max: " << strerror(errno) << endl;
    } else {
        fscanf(fp, "%llu", &pid_max_);
    }

    buf_ = new char[BUF_SIZE];
}

VmBase::~VmBase()
{
    delete[] buf_;
}

VmBase *VmBase::get_instance()
{
    static VmBase vm_base;
    return &vm_base;
}

void VmBase::set_vm_cmd(std::string vm_cmd)
{
    vm_ids_.clear();
    vm_total_mem_size_.clear();
    vm_cmd_ = vm_cmd;
}

std::set<VmId> VmBase::get_vm_ids()
{
    //TODO make sure already inited
    if(!joinable())
        refresh();
    return vm_ids_;
}

int VmBase::get_vm_total_mem_size(VmId vm_id)
{
    //TODO make sure already inited
    if(!joinable())
        refresh();
    return vm_total_mem_size_[vm_id];
}

void VmBase::refresh()
{
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

        //add vm_total_mem_size
        auto iter = find(args.begin(), args.end(), "-m");
        int size = -1;
        if (iter != args.end())
        {
            ++iter;
            size = stoi(*iter);
        }
        vm_total_mem_size_[vm_id] = size;
    }
    pclose(data);
    
    return;

}

void VmBase::run()
{
    while(!stop_)
    {
        refresh();
        this_thread::sleep_for(chrono::seconds(1));
    }
}

bool operator==(const VmId &lv, const VmId &rv)
{
    return lv.start_timestamp == rv.start_timestamp
        && lv.pid == rv.pid;
}

bool operator<(const VmId &lv, const VmId &rv)
{
    if(lv.start_timestamp == rv.start_timestamp)
        return lv.pid < rv.pid;
    else
        return lv.start_timestamp < rv.start_timestamp;
}
