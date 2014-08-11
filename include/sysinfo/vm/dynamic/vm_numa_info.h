#ifndef __NUMAINFO_H__
#define __NUMAINFO_H__

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

const int MAX_CACHE_LEVEL = 4;
const int MAX_NODE_NUM    = 8;
const int MAX_LLC_NUM     = 16;
const int MAX_PU_PER_LLC  = 32;
const int MAX_PU_NUM      = MAX_LLC_NUM * MAX_PU_PER_LLC;

/**
 * NUMA memory policy of memory blocks in <code>numa_maps</code>
 */
enum class NUMAPolicy {
    DEFAULT, PREFER
};

/**
 * Memory block type in <code>numa_maps</code>
 */
enum class MemType {
    STACK, HEAP, WTF
};

/**
 * Mmeory block information in <code>numa_maps</code>
 * @author zsy
 */
struct MemBlock {
    uint64_t address; 
    NUMAPolicy numa_policy;
    MemType mem_type;
    /**
     * Mapped file
     */
    std::string file;
    int anon;
    int dirty;
    int active;
    int mapped;
    int mapmax;
    int pages_sum;
    int prefer_node;
    int num_pages_on_node[MAX_NODE_NUM];
    MemBlock();
    /**
     * Dump internal data to screen.
     */
    void dump() const;
};

/**
 * Util functions of parsing <code>numa_maps</code>
 * @author zsy
 */
namespace NUMAMaps {
    /**
     * Read and parse <code>numa_maps</code> file.
     * @param <code>tgid</code>  task group id (process id)
     * @param <code>mem_blocks</code>  output parsed indormation
     */
    void get_mem_blocks(int tgid, std::vector<MemBlock>* mem_blocks);
    void count_pages_on_nodes(const std::vector<MemBlock>& mem_blocks,
            int pages_on_node[]);
    void count_pages_on_nodes(int tgid, int pages_on_node[]);
     /**
     * @param <code>mem_blocks</code>  parsed numa_maps information.
     * @return The node id with the most memory pages of <code>tgid</code> on 
     */
    int node_with_max_mem(const std::vector<MemBlock>& mem_blocks);
    void pages_num4nodes(int tgid, std::vector<int>& ret);
    /**
     * @param <code>tgid</code>
     * @return The node id with the most memory pages of <code>tgid</code> on 
     */
    int node_with_max_mem(int tgid);
    /**
     * @param <code>mem_blocks</code>  parsed numa_maps information.
     * @return The node id with the most memory pages of <code>tgid</code> on
     * with weight for different type of memory block. 
     */
    int node_with_max_weighted_mem(const std::vector<MemBlock>& mem_blocks);
    /**
     * @param <code>tgid</code>
     * @return The node id with the most memory pages of <code>tgid</code> on
     * with weight for different type of memory block. 
     */
    int node_with_max_weighted_mem(int tgid);
    /**
     * @return If NUMA policy is `prefered`, return the prefered node id.
     * Else return the node with max mem page on. 
     */
    int get_prefered_node(int tgid);
    /**
     * Count the number of page number of a thread except 
     * those on node with <code>nodeid</code>
     */
    int count_page_except_node(int tgid, int nodeid);
    /**
     * Count the number of page number of a thread except 
     * those on node with <code>nodeid</code>
     */
    int count_page_except_node(const std::vector<MemBlock>& mem_blocks, int nodeid);
}

int get_proc_numa_dist(int pid,uint64_t *pages,int nnodes);

#endif
