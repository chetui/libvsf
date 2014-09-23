#include "sysinfo/host/static/node_sys_dist.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "utils/log.h"

NodeSysDist::NodeSysDist()
{
}

NodeSysDist *NodeSysDist::get_instance()
{
    static NodeSysDist node_sys_dist;
    return &node_sys_dist;
}

std::vector< std::vector<int> > NodeSysDist::get_node_sys_dist()
{
    std::shared_lock<std::shared_timed_mutex> read_lock(data_mutex_);
    if(has_data_)
        return node_sys_dist_;
    read_lock.unlock();
    std::unique_lock<std::shared_timed_mutex> write_lock(data_mutex_);
    has_data_ = true;
    node_sys_dist_.clear();

    std::string command = "numactl --hardware";
    // execuate command
    FILE *fp = popen(command.c_str(), "r");
    
    // read result
    if(fp == NULL)
        return node_sys_dist_;
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
    str_tools::split(res, '\n', res_in_line);
    std::vector<std::string>::iterator i = res_in_line.begin();
    i++; // ignore the first line "node 0 1 ..."
    for(; i!= res_in_line.end(); i++)
    {
        std::vector<std::string> split_res;
        std::vector<int> line;
        str_tools::split(*i, ' ', split_res); // e.g. ["0:", "10", "20"]
        for(unsigned int j=1; j<split_res.size(); j++)
        {
            line.push_back(atoi(split_res[j].c_str()));
        }
        node_sys_dist_.push_back(line);
    }
    fclose(fp);
    return node_sys_dist_;
}

int NodeSysDist::get_node_sys_dist(int node_id_0, int node_id_1)
{
    return get_node_sys_dist()[node_id_0][node_id_1];
}

void NodeSysDist::clear()
{
    std::unique_lock<std::shared_timed_mutex> lock(data_mutex_);
    node_sys_dist_.clear();
    has_data_ = false;
}

void NodeSysDist::refresh()
{
    get_node_sys_dist();
}
