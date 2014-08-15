#include "sysinfo/host/static/node_cpu.h"

#include <unistd.h>

#include <cstdlib>
#include <cctype>
#include <cstring>
#include <fstream>

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
    lock_guard<mutex> lock(nodes_mutex_);
    do_refresh();
    return nodes_.size();
}

int NodeCpu::puid_to_nodeid(int puid) const
{
    for (size_t i = 0; i < nodes_.size(); ++i) {
        if (nodes_[i].find(puid) != nodes_[i].end()) {
            return i;
        }
    }
    return -1;
}

int NodeCpu::get_pu_num() const
{
    int num_conf   = sysconf(_SC_NPROCESSORS_CONF);
    int num_online = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_conf < num_online) {
        num_conf = num_online;
    }
    return num_conf;
}

int NodeCpu::get_pu_num_on_node(int nodeid) const
{
    return nodes_[nodeid].size();
}

pus_t& NodeCpu::get_pu_on_node(int nodeid) const
{
    return nodes_[nodeid];
}
void NodeCpu::refresh()
{
    lock_guard<mutex> lock(nodes_mutex_);
    do_refresh();
}
void NodeCpu::do_refresh()
{
    if(inited_)
        return;
    inited_ = true;

    vector<string> node_dirs;
    str_tools::get_dirs(NODE_DIR, "node", &node_dirs);
    nodes_ = vector<set<int>>(node_dirs.size()); 
    for (size_t i=0; i<node_dirs.size(); ++i) {
        string cpulist;
//#ifdef HARDWARE_DEBUG
//        LOG() << "reading from " << NODE_DIR + node_dirs[i] << endl;
//#endif
        ifstream fin(NODE_DIR + node_dirs[i] + "/cpulist");
        if (!fin.good()) {
//            LOG(LogLevel::err) 
//                << "NodeCpu::refresh: " << strerror(errno) << endl;
            return;
        }
        fin >> cpulist;
//        int cpu_id = 0;
//        for (auto c : cpulist) {
//            if (c == ',') {
//                nodes_[i].insert(cpu_id); 
//                cpu_id = 0;
//            } else {
//                cpu_id = cpu_id * 10 + (c - '0');
//            }
//        }
//        nodes_[i].insert(cpu_id);
        int beg_idx = 0;
        int cnt = 0;
        int clsize = cpulist.size();
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
                            nodes_[i].insert(j);
                        break;
                    }
                    cnt2++;
                }
                if(!has)
                    nodes_[i].insert(stoi(substri));
                beg_idx = cnt + 1;
            }
            cnt++;
        }
//        for(auto& x : nodes_[i])
//              LOG() << "node[" << i << "]:" << x;
    }
}


