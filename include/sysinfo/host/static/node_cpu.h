#ifndef _HOST_NODE_CPU_H_
#define _HOST_NODE_CPU_H_

#include <vector>
#include <string>
#include <set>
#include <mutex>

using pus_t = const std::set<int>;

/**
 * Singlton
 */
class NodeCpu {
public:
    ~NodeCpu();
    /**
     * not thread-safe
     */
    static NodeCpu* get_instance();
    /**
     * Get NUMA node number on this system.
     */
    int get_node_num();
    /**
     * Given id of process unit, return the NUMA node id where the
     * process unit located.
     */
    int puid_to_nodeid(int puid) const;
    /**
     * Get system wide number of process units.
     */
    int get_pu_num() const;
    /**
     * Given the id of node, return the number of process units on it.
     */
    int get_pu_num_on_node(int nodeid) const;
    /**
     * Get a set of process unit id on the given NUMA node id.
     */
    pus_t& get_pu_on_node(int nodeid) const;

    /**
     * Reread the node_cpu info.
     */
    void refresh();

private:
    NodeCpu();
    void do_refresh();

    std::vector<std::set<int> > nodes_;
    bool inited_ = false;
    std::mutex nodes_mutex_;


    static constexpr char const * NODE_DIR = "/sys/devices/system/node/";
};


#endif
