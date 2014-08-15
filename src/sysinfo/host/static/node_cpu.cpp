#include "sysinfo/host/static/node_cpu.h"

#include <unistd.h>

#include <cstdlib>
#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "utils/str_tools.h"

using namespace std;

NodeCpu::NodeCpu()
{
}

NodeCpu::~NodeCpu()
{
}

NodeCpu* NodeCpu::get_instance()
{
    static NodeCpu node_cpu;
    return &node_cpu;
}

int NodeCpu::get_node_num()
{
    lock_guard<recursive_mutex> lock(node_hpthread_mutex_);
    refresh();
    return node_hpthread_.size();
}

std::set<NodeId> NodeCpu::get_node_ids() 
{
    lock_guard<recursive_mutex> lock(node_hpthread_mutex_);
    refresh();
    set<NodeId> ids;
    for (auto& x : node_hpthread_)
        ids.insert(x.first);
    return ids;
}
NodeId NodeCpu::get_node_id(SocketId socket_id) 
{
    lock_guard<recursive_mutex> lock(node_socket_mutex_);
    refresh();
    for (auto& node_socket : node_socket_) {
        if (node_socket.second.find(socket_id) != node_socket.second.end()) {
            return node_socket.first;
        }
    }
    return {};
}
NodeId NodeCpu::get_node_id(CoreId core_id) 
{
    lock_guard<recursive_mutex> lock(node_core_mutex_);
    refresh();
    for (auto& node_core : node_core_) {
        if (node_core.second.find(core_id) != node_core.second.end()) {
            return node_core.first;
        }
    }
    return {};
}
NodeId NodeCpu::get_node_id(HpthreadId hpthread_id) 
{
    lock_guard<recursive_mutex> lock(node_hpthread_mutex_);
    refresh();
    for (auto& node_hpthread : node_hpthread_) {
        if (node_hpthread.second.find(hpthread_id) != node_hpthread.second.end()) {
            return node_hpthread.first;
        }
    }
    return {};
}
int NodeCpu::get_socket_num() 
{
    lock_guard<recursive_mutex> lock(socket_hpthread_mutex_);
    refresh();
    return socket_hpthread_.size();
}
int NodeCpu::get_socket_num(NodeId node_id) 
{
    lock_guard<recursive_mutex> lock(node_socket_mutex_);
    refresh();
    if (node_socket_.find(node_id) != node_socket_.end())
        return node_socket_[node_id].size();
    else
        return 0;
}
std::set<SocketId> NodeCpu::get_socket_ids() 
{
    lock_guard<recursive_mutex> lock(socket_hpthread_mutex_);
    refresh();
    set<SocketId> ids;
    for (auto& x : socket_hpthread_)
        ids.insert(x.first);
    return ids;
}
std::set<SocketId> NodeCpu::get_socket_ids(NodeId node_id) 
{
    lock_guard<recursive_mutex> lock(node_socket_mutex_);
    refresh();
    if (node_socket_.find(node_id) != node_socket_.end())
        return node_socket_[node_id];
    else
        return {};
}
SocketId NodeCpu::get_socket_id(CoreId core_id) 
{
    return SocketId(core_id.socket_id);
//    lock_guard<recursive_mutex> lock(socket_core_mutex_);
//    refresh();
//    for (auto& socket_core : socket_core_) {
//        if (socket_core.find(core_id) != socket_core.end()) {
//            return socket_core.first;
//        }
//    }
//    return {};
}
SocketId NodeCpu::get_socket_id(HpthreadId hpthread_id) 
{
    lock_guard<recursive_mutex> lock(socket_hpthread_mutex_);
    refresh();
    for (auto& socket_hpthread : socket_hpthread_) {
        if (socket_hpthread.second.find(hpthread_id) != socket_hpthread.second.end()) {
            return socket_hpthread.first;
        }
    }
    return {};
}
int NodeCpu::get_core_num() 
{
    lock_guard<recursive_mutex> lock(core_hpthread_mutex_);
    refresh();
    return core_hpthread_.size();
}
int NodeCpu::get_core_num(NodeId node_id) 
{
    lock_guard<recursive_mutex> lock(node_core_mutex_);
    refresh();
    if (node_core_.find(node_id) != node_core_.end())
        return node_core_[node_id].size();
    else
        return 0;
}
int NodeCpu::get_core_num(SocketId socket_id) 
{
    lock_guard<recursive_mutex> lock(socket_core_mutex_);
    refresh();
    if (socket_core_.find(socket_id) != socket_core_.end())
        return socket_core_[socket_id].size();
    else
        return 0;
}
std::set<CoreId> NodeCpu::get_core_ids() 
{
    lock_guard<recursive_mutex> lock(core_hpthread_mutex_);
    refresh();
    set<CoreId> ids;
    for (auto& x : core_hpthread_)
        ids.insert(x.first);
    return ids;
}
std::set<CoreId> NodeCpu::get_core_ids(NodeId node_id) 
{
    lock_guard<recursive_mutex> lock(node_core_mutex_);
    refresh();
    if (node_core_.find(node_id) != node_core_.end())
        return node_core_[node_id];
    else
        return {};
}
std::set<CoreId> NodeCpu::get_core_ids(SocketId socket_id) 
{
    lock_guard<recursive_mutex> lock(socket_core_mutex_);
    refresh();
    if (socket_core_.find(socket_id) != socket_core_.end())
        return socket_core_[socket_id];
    else
        return {};
}
CoreId NodeCpu::get_core_id(HpthreadId hpthread_id) 
{
    lock_guard<recursive_mutex> lock(core_hpthread_mutex_);
    refresh();
    for (auto& core_hpthread : core_hpthread_) {
        if (core_hpthread.second.find(hpthread_id) != core_hpthread.second.end()) {
            return core_hpthread.first;
        }
    }
    return {};
}
int NodeCpu::get_hpthread_num() 
{
    lock_guard<recursive_mutex> lock(hpthread_mutex_);
    refresh();
    return hpthread_.size();
}
int NodeCpu::get_hpthread_num(NodeId node_id) 
{
    lock_guard<recursive_mutex> lock(node_hpthread_mutex_);
    refresh();
    if (node_hpthread_.find(node_id) != node_hpthread_.end())
        return node_hpthread_[node_id].size();
    else
        return 0;
}
int NodeCpu::get_hpthread_num(SocketId socket_id) 
{
    lock_guard<recursive_mutex> lock(socket_hpthread_mutex_);
    refresh();
    if (socket_hpthread_.find(socket_id) != socket_hpthread_.end())
        return socket_hpthread_[socket_id].size();
    else
        return 0;
}
int NodeCpu::get_hpthread_num(CoreId core_id) 
{
    lock_guard<recursive_mutex> lock(core_hpthread_mutex_);
    refresh();
    if (core_hpthread_.find(core_id) != core_hpthread_.end())
        return core_hpthread_[core_id].size();
    else
        return 0;
}
std::set<HpthreadId> NodeCpu::get_hpthread_ids() 
{
    lock_guard<recursive_mutex> lock(hpthread_mutex_);
    refresh();
    return hpthread_;
}
std::set<HpthreadId> NodeCpu::get_hpthread_ids(NodeId node_id) 
{
    lock_guard<recursive_mutex> lock(node_hpthread_mutex_);
    refresh();
    if (node_hpthread_.find(node_id) != node_hpthread_.end())
        return node_hpthread_[node_id];
    else
        return {};
}
std::set<HpthreadId> NodeCpu::get_hpthread_ids(SocketId socket_id) 
{
    lock_guard<recursive_mutex> lock(socket_hpthread_mutex_);
    refresh();
    if (socket_hpthread_.find(socket_id) != socket_hpthread_.end())
        return socket_hpthread_[socket_id];
    else
        return {};
}
std::set<HpthreadId> NodeCpu::get_hpthread_ids(CoreId core_id) 
{
    lock_guard<recursive_mutex> lock(core_hpthread_mutex_);
    refresh();
    if (core_hpthread_.find(core_id) != core_hpthread_.end())
        return core_hpthread_[core_id];
    else
        return {};
}

void NodeCpu::refresh()
{
    lock_guard<recursive_mutex> node_socket_lock(node_socket_mutex_);
    lock_guard<recursive_mutex> node_core_lock(node_core_mutex_);
    lock_guard<recursive_mutex> node_hpthread_lock(node_hpthread_mutex_);
    lock_guard<recursive_mutex> socket_core_lock(socket_core_mutex_);
    lock_guard<recursive_mutex> socket_hpthread_lock(socket_hpthread_mutex_);
    lock_guard<recursive_mutex> core_hpthread_lock(core_hpthread_mutex_);
    lock_guard<recursive_mutex> hpthread_lock(hpthread_mutex_);
    if(inited_)
        return;
    inited_ = true;

    //read cpulist
    vector<string> node_dirs;
    str_tools::get_dirs(NODE_DIR, "node", &node_dirs);
    for (size_t i = 0; i < node_dirs.size(); ++i) {
        string cpulist;
        ifstream fin(NODE_DIR + node_dirs[i] + "/cpulist");
        if (!fin.good()) {
            //TODO throw
//            LOG(LogLevel::err) 
//                << "NodeCpu::refresh: " << strerror(errno) << endl;
            return;
        }
        fin >> cpulist;
//        int cpu_id = 0;
//        for (auto c : cpulist) {
//            if (c == ',') {
//                node_hpthread_[i].insert(cpu_id); 
//                cpu_id = 0;
//            } else {
//                cpu_id = cpu_id * 10 + (c - '0');
//            }
//        }
//        node_hpthread_[i].insert(cpu_id);
        int beg_idx = 0;
        int cnt = 0;
        int clsize = cpulist.size();
        int node_id = stoi(node_dirs[i].substr(4));
        for (auto c : cpulist) {
            if (c == ',' || cnt+1 == clsize) {
                if(cnt+1 == clsize)
                    cnt++;
                string substri = cpulist.substr(beg_idx, cnt);
                int cnt2 = 0;
                bool has = false;
                for(auto sc : substri) {
                    if(sc == '-') {
                        has = true;
                        int cpu_beg = stoi(substri.substr(0,cnt2));
                        int cpu_end = stoi(substri.substr(cnt2+1, substri.size()));
                        for(int j = cpu_beg; j < cpu_end+1; ++j)
                            node_hpthread_[node_id].insert(j);
                        break;
                    }
                    cnt2++;
                }
                if(!has)
                    node_hpthread_[node_id].insert(stoi(substri));
                beg_idx = cnt + 1;
            }
            cnt++;
        }
    }

    //read cpuinfo
    ifstream fin(CPUINFO_FILE);
    if(!fin.good()) {
        //TODO throw
        return;
    }
    string s;
    HpthreadId hpthread_id;
    SocketId socket_id;
    CoreId core_id;
    while (getline(fin, s)) {
        vector<string> words;
        if (str_tools::strip(s) != "") {
            str_tools::split(s, ':', words);
            for (auto& w : words)
            if (str_tools::strip(words[0]) == "processor") {
                hpthread_id = HpthreadId(stoi(words[1]));
            } else if (str_tools::strip(words[0]) == "physical id") {
                socket_id = SocketId(stoi(words[1]));
            } else if (str_tools::strip(words[0]) == "core id") {
                core_id = CoreId(socket_id.id, stoi(words[1]));
            }
        } else {
            //insert
            socket_hpthread_[socket_id].insert(hpthread_id);
            core_hpthread_[core_id].insert(hpthread_id);
            hpthread_.insert(hpthread_id);
        }
    }

    //reasoning 
    for (auto& socket_hpthread : socket_hpthread_) {
        for (auto& node_hpthread : node_hpthread_) {
            if (includes(node_hpthread.second.begin(), node_hpthread.second.end(),
                    socket_hpthread.second.begin(), socket_hpthread.second.end())) {
                node_socket_[node_hpthread.first].insert(socket_hpthread.first);
            }
        }
    }
    for (auto& core_hpthread : core_hpthread_) {
        for (auto& node_hpthread : node_hpthread_) {
            if (includes(node_hpthread.second.begin(), node_hpthread.second.end(),
                    core_hpthread.second.begin(), core_hpthread.second.end())) {
                node_core_[node_hpthread.first].insert(core_hpthread.first);
            }
        }
    }
    for (auto& core_hpthread : core_hpthread_) {
        for (auto& socket_hpthread : socket_hpthread_) {
            if (includes(socket_hpthread.second.begin(), socket_hpthread.second.end(),
                    core_hpthread.second.begin(), core_hpthread.second.end())) {
                socket_core_[socket_hpthread.first].insert(core_hpthread.first);
            }
        }
    }
}


bool operator==(const NodeId& lc, const NodeId& rc)
{
    return lc.id == rc.id;
}

bool operator<(const NodeId& lc, const NodeId& rc)
{
    return lc.id < rc.id;
}

std::ostream &operator<<(std::ostream &os, const NodeId &v)
{
    os << v.id;
    return os;
}

bool operator==(const SocketId& lc, const SocketId& rc)
{
    return lc.id == rc.id;
}

std::ostream &operator<<(std::ostream &os, const SocketId &v)
{
    os << v.id;
    return os;
}

bool operator<(const SocketId& lc, const SocketId& rc)
{
    return lc.id < rc.id;
}

bool operator==(const CoreId& lc, const CoreId& rc)
{
    return lc.socket_id == rc.socket_id 
        && lc.core_id == rc.core_id;
}

bool operator<(const CoreId& lc, const CoreId& rc)
{
    if (lc.socket_id < rc.socket_id)
        return true;
    else if (lc.socket_id == rc.socket_id)
        return lc.core_id < rc.core_id;
    else return false;
}

std::ostream &operator<<(std::ostream &os, const CoreId &v)
{
    os << v.socket_id << ":" << v.core_id;
    return os;
}

bool operator==(const HpthreadId& lc, const HpthreadId& rc)
{
    return lc.id == rc.id;
}

bool operator<(const HpthreadId& lc, const HpthreadId& rc)
{
    return lc.id < rc.id;
}

std::ostream &operator<<(std::ostream &os, const HpthreadId &v)
{
    os << v.id;
    return os;
}

