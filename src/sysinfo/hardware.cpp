#include "sysinfo/hardware.h"

#include <unistd.h>
#include <dirent.h>

#include <cstdlib>
#include <cctype>
#include <cstring>
#include <fstream>

using namespace std;

const char* NODE_DIR = "/sys/devices/system/node/";

Hardware::Hardware()
{
    refresh();
}

Hardware::~Hardware()
{
}

bool Hardware::compat_checking()
{
    //TODO
    return true;
}

Hardware* Hardware::get_instance()
{
    if (!compat_checking())
        return nullptr;

    static Hardware hardware;
    return &hardware;
}

int Hardware::get_node_num() const
{
    return nodes_.size();
}

int Hardware::puid_to_nodeid(int puid) const
{
    for (size_t i = 0; i < nodes_.size(); ++i) {
        if (nodes_[i].find(puid) != nodes_[i].end()) {
            return i;
        }
    }
    return -1;
}

int Hardware::get_pu_num() const
{
    int num_conf   = sysconf(_SC_NPROCESSORS_CONF);
    int num_online = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_conf < num_online) {
        num_conf = num_online;
    }
    return num_conf;
}

int Hardware::get_pu_num_on_node(int nodeid) const
{
    return nodes_[nodeid].size();
}

pus_t& Hardware::get_pu_on_node(int nodeid) const
{
    return nodes_[nodeid];
}

void Hardware::refresh()
{
    vector<string> node_dirs;
    get_node_dirs(&node_dirs);
    nodes_ = vector<set<int>>(node_dirs.size()); 
    for (size_t i=0; i<node_dirs.size(); ++i) {
        string cpulist;
//#ifdef HARDWARE_DEBUG
//        LOG() << "reading from " << NODE_DIR + node_dirs[i] << endl;
//#endif
        ifstream fin(string(NODE_DIR).append(node_dirs[i]).append("/cpulist"));
        if (!fin.good()) {
//            LOG(LogLevel::err) 
//                << "Hardware::refresh: " << strerror(errno) << endl;
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

int Hardware::node_and_digits(const struct dirent *dptr)
{
    auto *p = (char *)(dptr->d_name);
    if (*p++ != 'n') return 0;
    if (*p++ != 'o') return 0;
    if (*p++ != 'd') return 0;
    if (*p++ != 'e') return 0;
    do {
        if (!isdigit(*p++))
            return 0;
    } while (*p != '\0');
    return 1;
}

void Hardware::get_node_dirs(vector<string>* node_dirs)
{
    struct dirent **namelist;
    int num_nodes = scandir(NODE_DIR, &namelist, Hardware::node_and_digits, nullptr);
    if (num_nodes < 0) {
//        LOG(LogLevel::err) 
//            << "Hardware::get_node_dirs: " << strerror(errno) << endl;
    } else {
        for (int i=0; i<num_nodes; ++i) {
            node_dirs->push_back(string(namelist[i]->d_name));
            free(namelist[i]);
        }
        free(namelist);
    }
}

