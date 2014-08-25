#include "sysinfo/host/static/node_cpu.h"

#include <unistd.h>

#include <cstdlib>
#include <cctype>
#include <cstring>
#include <fstream>
#include <algorithm>

#include "utils/str_tools.h"
#include "framework/exception.h"

using namespace std;

NodeCpu::NodeCpu(): inited_(false)
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

template <typename K, typename V>
std::set<K> NodeCpu::get_keys(std::map<K, std::set<V> >& data)
{
    refresh();
    shared_lock<shared_timed_mutex> lock(data_mutex_);
    set<K> ids;
    for (auto& x : data)
        ids.insert(x.first);
    return ids;
}

template <typename T>
std::set<T> NodeCpu::get_keys(std::set<T>& data)
{
    refresh();
    shared_lock<shared_timed_mutex> lock(data_mutex_);
    return data;
}

template <typename T>
int NodeCpu::get_key_num(T& data)
{
    refresh();
    shared_lock<shared_timed_mutex> lock(data_mutex_);
    return data.size();
}

template <typename K, typename V>
K NodeCpu::get_key_by_value(std::map<K, std::set<V> >& data, const V& value)
{
    refresh();
    shared_lock<shared_timed_mutex> lock(data_mutex_);
    for (auto& d : data) {
        if (d.second.find(value) != d.second.end()) {
            return d.first;
        }
    }
    return {};
}

template <typename K, typename V>
std::set<V> NodeCpu::get_value_by_key(std::map<K, std::set<V> >& data, const K& key)
{
    refresh();
    shared_lock<shared_timed_mutex> lock(data_mutex_);
    if (data.find(key) != data.end())
        return data[key];
    else
        return {};
}

template <typename K, typename V>
int NodeCpu::get_value_num_by_key(std::map<K, std::set<V> >& data, const K& key)
{
    refresh();
    shared_lock<shared_timed_mutex> lock(data_mutex_);
    if (data.find(key) != data.end())
        return data[key].size();
    else
        return 0;
}

int NodeCpu::get_node_num()
{
    return get_key_num(node_hpthread_);
}

std::set<NodeId> NodeCpu::get_node_ids() 
{
    return get_keys(node_hpthread_);
}
NodeId NodeCpu::get_node_id(SocketId socket_id) 
{
    return get_key_by_value(node_socket_, socket_id);
}
NodeId NodeCpu::get_node_id(CoreId core_id) 
{
    return get_key_by_value(node_core_, core_id);
}
NodeId NodeCpu::get_node_id(HpthreadId hpthread_id) 
{
    return get_key_by_value(node_hpthread_, hpthread_id);
}
int NodeCpu::get_socket_num() 
{
    return get_key_num(socket_hpthread_);
}
int NodeCpu::get_socket_num(NodeId node_id) 
{
    return get_value_num_by_key(node_socket_, node_id);
}
std::set<SocketId> NodeCpu::get_socket_ids() 
{
    return get_keys(socket_hpthread_);
}
std::set<SocketId> NodeCpu::get_socket_ids(NodeId node_id) 
{
    return get_value_by_key(node_socket_, node_id);
}
SocketId NodeCpu::get_socket_id(CoreId core_id) 
{
    return SocketId(core_id.socket_id);
//    return get_key_by_value(socket_core_, core_id);
}
SocketId NodeCpu::get_socket_id(HpthreadId hpthread_id) 
{
    return get_key_by_value(socket_hpthread_, hpthread_id);
}
int NodeCpu::get_core_num() 
{
    return get_key_num(core_hpthread_);
}
int NodeCpu::get_core_num(NodeId node_id) 
{
    return get_value_num_by_key(node_core_, node_id);
}
int NodeCpu::get_core_num(SocketId socket_id) 
{
    return get_value_num_by_key(socket_core_, socket_id);
}
std::set<CoreId> NodeCpu::get_core_ids() 
{
    return get_keys(core_hpthread_);
}
std::set<CoreId> NodeCpu::get_core_ids(NodeId node_id) 
{
    return get_value_by_key(node_core_, node_id);
}
std::set<CoreId> NodeCpu::get_core_ids(SocketId socket_id) 
{
    return get_value_by_key(socket_core_, socket_id);
}
CoreId NodeCpu::get_core_id(HpthreadId hpthread_id) 
{
    return get_key_by_value(core_hpthread_, hpthread_id);
}
int NodeCpu::get_hpthread_num() 
{
    return get_key_num(hpthread_);
}
int NodeCpu::get_hpthread_num(NodeId node_id) 
{
    return get_value_num_by_key(node_hpthread_, node_id);
}
int NodeCpu::get_hpthread_num(SocketId socket_id) 
{
    return get_value_num_by_key(socket_hpthread_, socket_id);
}
int NodeCpu::get_hpthread_num(CoreId core_id) 
{
    return get_value_num_by_key(core_hpthread_, core_id);
}
std::set<HpthreadId> NodeCpu::get_hpthread_ids() 
{
    return get_keys(hpthread_);
}
std::set<HpthreadId> NodeCpu::get_hpthread_ids(NodeId node_id) 
{
    return get_value_by_key(node_hpthread_, node_id);
}
std::set<HpthreadId> NodeCpu::get_hpthread_ids(SocketId socket_id) 
{
    return get_value_by_key(socket_hpthread_, socket_id);
}
std::set<HpthreadId> NodeCpu::get_hpthread_ids(CoreId core_id) 
{
    return get_value_by_key(core_hpthread_, core_id);
}

void NodeCpu::refresh()
{
    if(inited_)
        return;

    unique_lock<shared_timed_mutex> lock(data_mutex_);

    refresh_node_hpthread();
    refresh_socket_core_hpthread();
    refresh_others();

    inited_ = true;
}

void NodeCpu::refresh_node_hpthread()
{
    vector<string> node_dirs;
    str_tools::get_dirs(NODE_DIR, "node", &node_dirs);
    for (size_t i = 0; i < node_dirs.size(); ++i) {
        string file_path = NODE_DIR + node_dirs[i] + "/cpulist";
        ifstream fin(file_path);
        if (!fin.good()) {
            THROW(FileOpenFailed, "To open file: " + file_path);
            return;
        }
        string cpulist;
        fin >> cpulist;
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
}

void NodeCpu::refresh_socket_core_hpthread()
{
    ifstream fin(CPUINFO_FILE);
    if(!fin.good()) {
        THROW(FileOpenFailed, "To open file: " + string(CPUINFO_FILE));
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
}

void NodeCpu::refresh_others()
{
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
    os << v.socket_id << "-" << v.core_id;
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

