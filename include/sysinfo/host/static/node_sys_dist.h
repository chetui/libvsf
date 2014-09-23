#ifndef _HOST_NODE_SYS_DIST_H_
#define _HOST_NODE_SYS_DIST_H_

#include <vector>
#include <string>
#include <mutex>
#include <shared_mutex>
#include "utils/str_tools.h"

/**
 * multi-thread safe
 */
class NodeSysDist {
public:
    static NodeSysDist* get_instance();

    std::vector< std::vector<int> > get_node_sys_dist();
    int get_node_sys_dist(int node_id_0, int node_id_1);

    void refresh();
    void clear();

private:
    NodeSysDist();
    std::vector<std::vector<int> > node_sys_dist_;
    bool has_data_ = false;
    std::shared_timed_mutex data_mutex_;

    static constexpr int BUF_SIZE = 1024;
};


#endif
