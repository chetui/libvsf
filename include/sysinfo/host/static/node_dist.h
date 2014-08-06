#ifndef _HOST_NODE_DIST_H_
#define _HOST_NODE_DIST_H_

#include <vector>
#include <string>
#include <mutex>

#define WORKLOADTYPE_RANDOM 'r'
#define WORKLOADTYPE_SERIAL 's'

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
class NodeDist {
public:
    static NodeDist* get_instance();

    std::vector< std::vector<int> > get_sys_node_dist();
    int get_sys_node_dist(int node_id_0, int node_id_1);

    std::vector< std::vector<int> > get_test_node_dist(const MicroParam& param);
    int get_test_node_dist(int node_id_0, int node_id_1, const MicroParam& param);

    void refresh_sys();
    void refresh_test(const MicroParam &param);

private:
    NodeDist();
    std::vector<std::vector<int> > sys_node_dist_;
    std::vector<std::vector<int> > test_node_dist_;
    bool sys_inited_ = false;
    bool test_inited_ = false;
    std::mutex sys_mutex;
    std::mutex test_mutex;
    MicroParam test_param_;

    void split(std::string& s, char delim, std::vector<std::string>& ret);

    static constexpr int BUF_SIZE = 1024;
};


#endif
