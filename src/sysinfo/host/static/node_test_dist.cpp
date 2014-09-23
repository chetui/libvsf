#include "sysinfo/host/static/node_test_dist.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "utils/log.h"
#include "sysinfo/host/static/node_cpu.h"

NodeTestDist::NodeTestDist()
{
    node_cpu_ = NodeCpu::get_instance();
}

NodeTestDist *NodeTestDist::get_instance()
{
    static NodeTestDist node_test_dist;
    return &node_test_dist;
}

std::vector< std::vector<int> > NodeTestDist::get_node_test_dist(const MicroParam& param) 
{
    std::shared_lock<std::shared_timed_mutex> read_lock(data_mutex_);
    if(has_data_ && param == param_)
        return node_test_dist_;
    read_lock.unlock();
    std::unique_lock<std::shared_timed_mutex> write_lock(data_mutex_);
    has_data_ = true;
    param_ = param;
    node_test_dist_.clear();

//    LDEBUG << "loop in get_node_test_dist" << param.loop << std::endl;
//    LDEBUG << param.path << std::endl;
//    LDEBUG << param.size_in_mb << std::endl;
//    LDEBUG << param.type << std::endl;
//    LDEBUG << param.loop << std::endl;
    int node_size = node_cpu_->get_node_num();
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
                return node_test_dist_;

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
        node_test_dist_.push_back(row);
    }

    return node_test_dist_;
}

int NodeTestDist::get_node_test_dist(int node_id_0, int node_id_1, const MicroParam& param)
{
    return get_node_test_dist(param)[node_id_0][node_id_1];
}

void NodeTestDist::clear()
{
    std::unique_lock<std::shared_timed_mutex> lock(data_mutex_);
    node_test_dist_.clear();
    has_data_ = false;
}

void NodeTestDist::refresh(const MicroParam &param)
{
    get_node_test_dist(param);
}

bool operator==(const MicroParam &lp, const MicroParam &rp)
{
    return lp.path == rp.path &&
        lp.size_in_mb == rp.size_in_mb &&
        lp.type == rp.type &&
        lp.loop == rp.loop;
}
