#ifndef _HOST_NODE_TEST_DIST_H_
#define _HOST_NODE_TEST_DIST_H_

#include <vector>
#include <string>
#include <mutex>
#include <shared_mutex>
#include "utils/str_tools.h"

#define WORKLOADTYPE_RANDOM 'r'
#define WORKLOADTYPE_SERIAL 's'

class NodeCpu;

struct MicroParam {
    MicroParam() {};
    MicroParam(std::string path, int size_in_mb, char type, int loop):path(path), size_in_mb(size_in_mb), type(type), loop(loop) {};

    std::string path = ".";
    int size_in_mb = 20;
    char type = WORKLOADTYPE_RANDOM;
    int loop = 200;
};
bool operator==(const MicroParam &lp, const MicroParam &rp);

/**
 * multi-thread safe
 */
class NodeTestDist {
public:
    static NodeTestDist* get_instance();
    void set_param(const MicroParam& param);
    void clear_param();

    std::vector< std::vector<int> > get_node_test_dist();
    int get_node_test_dist(int node_id_0, int node_id_1);

    void refresh();
    void clear();

private:
    NodeTestDist();
    std::vector<std::vector<int> > node_test_dist_;
    bool has_data_ = false;
    std::shared_timed_mutex data_mutex_;

    MicroParam param_;
    MicroParam last_param_;

    NodeCpu* node_cpu_;

    static constexpr int BUF_SIZE = 1024;
};


#endif
