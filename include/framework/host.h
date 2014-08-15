#ifndef _HOST_H_
#define _HOST_H_

#include "utils/func_option.h"
#include "sysinfo/host/static/node_cpu.h"
#include "sysinfo/host/static/node_dist.h"

class Host {
public:
    static Host *get_instance();

    //OP_HS_NODE_CPU
    int node_num();
    //OP_HS_SYS_NODE_DIST
    std::vector<std::vector<int> > sys_node_dist();
    int sys_node_dist(int node_id_0, int node_id_1);
    //OP_HS_TEST_NODE_DIST
        //may with Option Parameters. if without Option Parameters, then use default parameters.
    std::vector<std::vector<int> > test_node_dist();
    int test_node_dist(int node_id_0, int node_id_1);
        //must without Option Parameters.
    std::vector<std::vector<int> > test_node_dist(const MicroParam &p);
    int test_node_dist(int node_id_0, int node_id_1, const MicroParam &p);

private:
    Host();

    FuncOption* func_option_;
    NodeCpu* node_cpu_;
    NodeDist* node_dist_;

    //OP_HS_TEST_NODE_DIST
    MicroParam get_param_test_node_dist();

};

#endif
