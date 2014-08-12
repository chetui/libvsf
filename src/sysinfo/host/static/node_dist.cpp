#include "sysinfo/host/static/node_dist.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "sysinfo/host/static/node_core_hpthread.h"

NodeDist::NodeDist()
{
}

NodeDist *NodeDist::get_instance()
{
    static NodeDist node_dist;
    return &node_dist;
}

std::vector< std::vector<int> > NodeDist::get_test_node_dist(const MicroParam& param) 
{
    std::lock_guard<std::mutex> lock(test_mutex);
//    std::cout << "AA" << std::endl;
    if(test_inited_ && param == test_param_)
        return test_node_dist_;
    test_inited_ = true;
    test_param_ = param;
    test_node_dist_.clear();

//    std::cout << "BB" << std::endl;
//    std::cout << "loop in get_test_node_dist" << param.loop << std::endl;
//    std::cout << param.path << std::endl;
//    std::cout << param.size_in_mb << std::endl;
//    std::cout << param.type << std::endl;
//    std::cout << param.loop << std::endl;
    int node_size = NodeCoreHpthread::get_instance()->get_node_num();
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
                return test_node_dist_;

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
        test_node_dist_.push_back(row);
    }

//    std::cout << "CC" << std::endl;
    return test_node_dist_;
}

int NodeDist::get_test_node_dist(int node_id_0, int node_id_1, const MicroParam& param)
{
    return get_test_node_dist(param)[node_id_0][node_id_1];
}

std::vector< std::vector<int> > NodeDist::get_sys_node_dist()
{
    std::lock_guard<std::mutex> lock(sys_mutex);
    if(sys_inited_)
        return sys_node_dist_;
    sys_inited_ = true;
    sys_node_dist_.clear();

    std::string command = "numactl --hardware";
    // execuate command
    FILE *fp = popen(command.c_str(), "r");
    
    // read result
    if(fp == NULL)
        return sys_node_dist_;
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
    std::vector< std::vector<int> > node_dist;
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
        sys_node_dist_.push_back(line);
    }
    fclose(fp);
    return sys_node_dist_;
}

int NodeDist::get_sys_node_dist(int node_id_0, int node_id_1)
{
    return get_sys_node_dist()[node_id_0][node_id_1];
}

void NodeDist::refresh_sys()
{
    get_sys_node_dist();
}

void NodeDist::refresh_test(const MicroParam &param)
{
    get_test_node_dist(param);
}

bool operator==(const MicroParam &lp, const MicroParam &rp)
{
    return lp.path == rp.path &&
        lp.size_in_mb == rp.size_in_mb &&
        lp.type == rp.type &&
        lp.loop == rp.loop;
}
