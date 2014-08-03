#include "sysinfo/host/static/node_core_hpthread.h"

#include <unistd.h>
#include <dirent.h>

#include <cstdlib>
#include <cctype>
#include <cstring>
#include <fstream>

using namespace std;

NodeCoreHpthread::NodeCoreHpthread()
{
    refresh();
}

NodeCoreHpthread::~NodeCoreHpthread()
{
}

bool NodeCoreHpthread::compat_checking()
{
    //TODO
    return true;
}

NodeCoreHpthread* NodeCoreHpthread::get_instance()
{
    if (!compat_checking())
        return nullptr;

    static NodeCoreHpthread node_core_hpthread;
    return &node_core_hpthread;
}

int NodeCoreHpthread::get_node_num() const
{
    return nodes_.size();
}

int NodeCoreHpthread::puid_to_nodeid(int puid) const
{
    for (size_t i = 0; i < nodes_.size(); ++i) {
        if (nodes_[i].find(puid) != nodes_[i].end()) {
            return i;
        }
    }
    return -1;
}

int NodeCoreHpthread::get_pu_num() const
{
    int num_conf   = sysconf(_SC_NPROCESSORS_CONF);
    int num_online = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_conf < num_online) {
        num_conf = num_online;
    }
    return num_conf;
}

int NodeCoreHpthread::get_pu_num_on_node(int nodeid) const
{
    return nodes_[nodeid].size();
}

pus_t& NodeCoreHpthread::get_pu_on_node(int nodeid) const
{
    return nodes_[nodeid];
}

std::vector< std::vector<int> > NodeCoreHpthread::get_node_dist(MicroParam& param) {
    int node_size = nodes_.size();
    std::vector< std::vector<int> > ret;
    for(int cpu=0; cpu<node_size; cpu++)
    {
        std::vector<int> row;
        double base_time;
        // get dist for each node
        for(int mem=0; mem<node_size; mem++)
        {
            char command[BUF_SIZE];
            sprintf(command,
                    "numactl --cpunodebind=%d --membind=%d %s/microbench -wb %dm -wt %c -l %d",
                    cpu, mem, param.path.c_str(), param.size_in_mb, param.type, param.loop);
            
            // execuate command
            FILE *fp = popen(command, "r");
            if(fp == NULL)
                return ret;

            // read result
            char buf[BUF_SIZE];
            while(!feof(fp))
            {
                if(fgets(buf, BUF_SIZE, fp) == NULL)
                    break;
            }
            double time;
            sscanf(buf, "%lf", &time);

            // normalize
            if(mem == cpu)
                base_time = time;
            row.push_back((int)(time/base_time*10));
            fclose(fp);
        }
        ret.push_back(row);
    }
    return ret;
}

std::vector< std::vector<int> > NodeCoreHpthread::get_node_dist()
{
    std::vector< std::vector<int> > ret;
    std::string command = "numactl --hardware";
    // execuate command
    FILE *fp = popen(command.c_str(), "r");
    
    // read result
    if(fp == NULL)
        return ret;
    std::string res;
    char buf[BUF_SIZE];
    bool dist_flag = false;
    while(!feof(fp))
    {
        if(fgets(buf, BUF_SIZE, fp) == NULL)
            break;
        if(dist_flag == true) // read distance
        {
            res += buf;            
        }
        if(strcmp(buf, "node distances:\n") == 0) // change flag when meeting this line
            dist_flag = true;
    }

    // text processing to get distance
    std::vector<std::string> res_in_line;
    split(res, '\n', res_in_line);
    std::vector< std::vector<int> > node_dist;
    std::vector<std::string>::iterator i = res_in_line.begin();
    i++; // ignore the first line "node 0 1 ..."
    for(; i!= res_in_line.end(); i++)
    {
        std::vector<std::string> split_res;
        std::vector<int> line;
        split(*i, ' ', split_res); // e.g. ["0:", "10", "20"]
        for(unsigned int j=1; j<split_res.size(); j++)
        {
            line.push_back(atoi(split_res[j].c_str()));
        }
        ret.push_back(line);
    }
    fclose(fp);
    return ret;
}

void NodeCoreHpthread::refresh()
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
//                << "NodeCoreHpthread::refresh: " << strerror(errno) << endl;
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

int NodeCoreHpthread::node_and_digits(const struct dirent *dptr)
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

void NodeCoreHpthread::get_node_dirs(vector<string>* node_dirs)
{
    struct dirent **namelist;
    int num_nodes = scandir(NODE_DIR, &namelist, NodeCoreHpthread::node_and_digits, nullptr);
    if (num_nodes < 0) {
//        LOG(LogLevel::err) 
//            << "NodeCoreHpthread::get_node_dirs: " << strerror(errno) << endl;
    } else {
        for (int i=0; i<num_nodes; ++i) {
            node_dirs->push_back(string(namelist[i]->d_name));
            free(namelist[i]);
        }
        free(namelist);
    }
}

void NodeCoreHpthread::split(std::string& s, char delim, std::vector<std::string>& ret)
{
    int len = s.size();
    std::string sub_str = "";
    for(int i=0; i<len; i++)
    {
        if(s.at(i) == delim)            
        {
            if(sub_str != "")
            {
                ret.push_back(sub_str);
                sub_str = "";
            }
        }
        else
            sub_str.push_back(s.at(i));
    }
}

