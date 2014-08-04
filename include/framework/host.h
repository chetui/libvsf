#ifndef _HOST_H_
#define _HOST_H_

#include "utils/func_option.h"
#include "sysinfo/host/static/node_core_hpthread.h"
#include "sysinfo/host/static/node_dist.h"

class Host {
public:
    static Host *get_instance();

    //OP_HS_NODE_CORE_HPTHREAD
    int node_num();
    //OP_HS_SYS_NODE_DIST
    std::vector<std::vector<int> > sys_node_dist();
    int sys_node_dist(int node_id_0, int node_id_1);
    std::vector<std::vector<int> > test_node_dist();
    int test_node_dist(int node_id_0, int node_id_1);

private:
    Host();

    FuncOption* func_option_;
    NodeCoreHpthread* node_core_hpthread_;
    NodeDist* node_dist_;
};

#endif
