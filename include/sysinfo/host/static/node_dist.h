#ifndef _HOST_NODE_DIST_H_
#define _HOST_NODE_DIST_H_

#include <vector>
#include <string>

struct MicroParam;

class NodeDist {
public:
    static NodeDist* get_instance();

    std::vector< std::vector<int> > get_node_dist();
    std::vector< std::vector<int> > get_node_dist(MicroParam& param);

private:
    NodeDist();
    void split(std::string& s, char delim, std::vector<std::string>& ret);

    static constexpr int BUF_SIZE = 1024;
};

enum MicroWorkloadType : char { MWT_RANDOM = 'r', MWT_SERIAL = 's' };

struct MicroParam {
    MicroParam() {};
    MicroParam(std::string path, int size_in_mb, MicroWorkloadType type, int loop):path(path), size_in_mb(size_in_mb), type(type), loop(loop) {};

    std::string path = ".";
    int size_in_mb = 20;
    MicroWorkloadType type = MWT_RANDOM;
    int loop = 200;
};

#endif
