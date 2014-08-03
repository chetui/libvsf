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

std::vector< std::vector<int> > NodeDist::get_node_dist(MicroParam& param) {
    int node_size = NodeCoreHpthread::get_instance()->get_node_num();
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

std::vector< std::vector<int> > NodeDist::get_node_dist()
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


void NodeDist::split(std::string& s, char delim, std::vector<std::string>& ret)
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

