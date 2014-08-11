#include <vector>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <inttypes.h>
#include <assert.h>
#include "sysinfo\vm\dynamic\vm_numa_info.h"

using namespace std;

inline NUMAPolicy get_numa_policy(const string& numa_policy_str);
inline int prefered_node(const string& numa_policy_str);
inline MemType get_mem_type(const string& get_mem_type);
inline bool is_attribute(const string& str);
inline void set_attributes(const string& str, MemBlock* mem_block);
inline bool has_prefer_block(const vector<MemBlock>& mem_blocks);
inline int top_prefer_node(const vector<MemBlock>& mem_blocks);

#define LOG(x) cerr

int NUMAMaps::count_page_except_node(int tgid, int nodeid)
{
    vector<MemBlock> mem_blocks;
    get_mem_blocks(tgid, &mem_blocks);
    return count_page_except_node(mem_blocks, nodeid);
}

int NUMAMaps::count_page_except_node(const vector<MemBlock>& mem_blocks, int nodeid)
{
    int pages = 0;
    for (auto& mem_block : mem_blocks) {
        for (int i=0; i<MAX_NODE_NUM; ++i) {
            if (i == nodeid || (mem_block.mem_type != MemType::STACK 
                        && mem_block.mem_type != MemType::HEAP)) {
                continue;
            }
            pages += mem_block.num_pages_on_node[i];
        }
    }
    return pages;
}

int NUMAMaps::node_with_max_weighted_mem(int tgid)
{
    vector<MemBlock> mem_blocks;
    get_mem_blocks(tgid, &mem_blocks);
    return node_with_max_weighted_mem(mem_blocks);
}

// FIXME: only return prefered node of the first block
int top_prefer_node(const vector<MemBlock>& mem_blocks)
{
    int node = -1;
    if (mem_blocks.size() > 0) {
        node = mem_blocks[0].prefer_node;
    }
    return node;
}

int NUMAMaps::get_prefered_node(int tgid)
{
    vector<MemBlock> mem_blocks;
    get_mem_blocks(tgid, &mem_blocks);
    int node = -1;
    if (has_prefer_block(mem_blocks)) {
        node = top_prefer_node(mem_blocks);
        LOG() << tgid << " prefer " << node << endl;
    } else {
        node = node_with_max_mem(mem_blocks);
    }
    return node;
}

// FIXME: only check the first block
bool has_prefer_block(const vector<MemBlock>& mem_blocks) 
{
    if (mem_blocks.size() > 0 
            && mem_blocks[0].numa_policy == NUMAPolicy::PREFER) {
        return true;
    }
    return false;
}

int NUMAMaps::node_with_max_weighted_mem(const vector<MemBlock>& mem_blocks)
{
    int pages_on_node[MAX_NODE_NUM];
    fill_n(pages_on_node, MAX_NODE_NUM, 0);
    for (auto& mem_block : mem_blocks) {
        if (mem_block.mem_type != MemType::STACK 
                && mem_block.mem_type != MemType::HEAP) {
            continue;
        }
        for (int i=0; i<MAX_NODE_NUM; ++i) {
            pages_on_node[i] += mem_block.num_pages_on_node[i];
        }
    }
    int max_page = 0;
    int max_node = -1;
    for (int i=0; i<MAX_NODE_NUM; ++i) {
        if (pages_on_node[i] > max_page) {
            max_page = pages_on_node[i];
            max_node = i;
        }
    }
    return max_node;
}

void NUMAMaps::pages_num4nodes(int tgid, vector<int>& ret)
{
    vector<MemBlock> mem_blocks;
    get_mem_blocks(tgid, &mem_blocks);

    int pages_on_node[MAX_NODE_NUM];
    count_pages_on_nodes(mem_blocks, pages_on_node);

    for(int i = 0; i < MAX_NODE_NUM; ++i)
        ret.push_back(pages_on_node[i]);
    return;
}
int NUMAMaps::node_with_max_mem(int tgid)
{
    vector<MemBlock> mem_blocks;
    get_mem_blocks(tgid, &mem_blocks);
    return node_with_max_mem(mem_blocks);
}

void NUMAMaps::count_pages_on_nodes(int tgid, int pages_on_node[])
{
    vector<MemBlock> mem_blocks;
    get_mem_blocks(tgid, &mem_blocks);
    count_pages_on_nodes(mem_blocks, pages_on_node);
}

void NUMAMaps::count_pages_on_nodes(const vector<MemBlock>& mem_blocks,
        int pages_on_node[])
{
    fill_n(pages_on_node, MAX_NODE_NUM, 0);
    for (auto& mem_block : mem_blocks) {
        for (int i=0; i<MAX_NODE_NUM; ++i) {
            if (mem_block.num_pages_on_node[i] > 0) {
                pages_on_node[i] += mem_block.num_pages_on_node[i];
            }
        }
    }
}

int NUMAMaps::node_with_max_mem(const vector<MemBlock>& mem_blocks)
{
    int pages_on_node[MAX_NODE_NUM];
    count_pages_on_nodes(mem_blocks, pages_on_node);
    int max_page = 0;
    int max_node = -1;
    for (int i=0; i<MAX_NODE_NUM; ++i) {
        if (pages_on_node[i] > max_page) {
            max_page = pages_on_node[i];
            max_node = i;
        }
    }
    return max_node;
}

MemBlock::MemBlock():
    address(0), numa_policy(NUMAPolicy::DEFAULT), mem_type(MemType::WTF),
    file(), anon(0), dirty(0), active(0), mapped(0), mapmax(0), pages_sum(0), prefer_node(-1)
{
    fill_n(num_pages_on_node, MAX_NODE_NUM, -1);
}

void MemBlock::dump() const
{
    cout << "address   : " << address << endl
        <<  "policy    : " << (int)numa_policy << endl
        <<  "type      : " << (int)mem_type << endl
        <<  "file      : " << file << endl
        <<  "anon      : " << anon << endl
        <<  "dirty     : " << dirty << endl
        <<  "active    : " << active << endl
        <<  "mapped    : " << mapped << endl
        <<  "mapmax    : " << mapmax << endl
        <<  "pages_sum : " << pages_sum << endl;
}

void NUMAMaps::get_mem_blocks(int tgid, vector<MemBlock>* mem_blocks)
{
    ifstream map_file(string("/proc/").append(to_string(tgid)).append("/numa_maps")); 
    if (!map_file.good()) {
        LOG(LogLevel::err) << "NUMAMaps::get_mem_blocks: open numa_maps tgid: " << tgid << " failed " << strerror(errno) << endl;
        return;
    }
    for (string line; getline(map_file, line); ) {
        MemBlock mem_block;
        istringstream linein(line);
        linein >> hex >> mem_block.address >> dec;
        string tmp;
        linein >> tmp;
        mem_block.numa_policy = get_numa_policy(tmp);
        if (mem_block.numa_policy == NUMAPolicy::PREFER) {
            mem_block.prefer_node = prefered_node(tmp);
        }
        // attributes or type
        while (linein) {
            linein >> tmp;
            if (is_attribute(tmp)) {
                set_attributes(tmp, &mem_block);
            } else {
                mem_block.mem_type = get_mem_type(tmp);
            }
        }
        // sum page num
        for (int i=0; i<MAX_NODE_NUM; ++i) {
            if (mem_block.num_pages_on_node[i] > 0) {
                mem_block.pages_sum += mem_block.num_pages_on_node[i];
            }
        }
        mem_blocks->push_back(mem_block);
    }
}

NUMAPolicy get_numa_policy(const string& numa_policy_str)
{
    NUMAPolicy numa_policy = NUMAPolicy::DEFAULT;
    if (numa_policy_str == "DEFAULT") {
        numa_policy = NUMAPolicy::DEFAULT;
    } else if (numa_policy_str.find("prefer") != string::npos) {
        numa_policy = NUMAPolicy::PREFER;
    }
    return numa_policy;
}

int prefered_node(const string& numa_policy_str)
{
    static const string prefer("prefer:");
    static const size_t size = prefer.size();
    int node = -1;
    size_t pos = numa_policy_str.find(prefer);
    if (pos != string::npos) {
        node = stoi(numa_policy_str.substr(pos+size));
    }
    return node;
}

MemType get_mem_type(const string& str)
{
    MemType mem_type = MemType::WTF;
    if (str == "stack") {
        mem_type = MemType::STACK;
    } else if (str == "heap") {
        mem_type = MemType::HEAP;
    }
    return mem_type;
}

bool is_attribute(const string& str)
{
    return str.find('=') != string::npos;
}

void set_attributes(const string& str, MemBlock* mem_block)
{
    assert(mem_block != nullptr);
    size_t eq_pos = str.find('=');
    assert(eq_pos != string::npos);
    string attr_name = str.substr(0, eq_pos); 
    string value = str.substr(eq_pos+1);
    if (attr_name == "file") {
        mem_block->file = value;
    } else if (attr_name == "anon") {
        mem_block->anon = stoi(value);
    } else if (attr_name == "dirty") {
        mem_block->dirty = stoi(value);
    } else if (attr_name == "active") {
        mem_block->active = stoi(value);
    } else if (attr_name == "mapped") {
        mem_block->mapped = stoi(value);
    } else if (attr_name == "mapmax") {
        mem_block->mapmax = stoi(value);
    } else if (attr_name[0] == 'N') {
        int node_num = stoi(attr_name.substr(1));
        assert(node_num >= 0 && node_num < MAX_NODE_NUM);
        mem_block->num_pages_on_node[node_num] = stoi(value);
    } else {
        LOG(LogLevel::warn) 
            << __FILE__ << "::set_attributes: unknow attribute name " 
            << attr_name << endl; 
    }
}

int get_proc_numa_dist(int pid,uint64_t *pages,int nnodes)
{
	vector<MemBlock> mem_blocks;
    NUMAMaps::get_mem_blocks(pid, &mem_blocks);
	
	memset(pages, 0, sizeof(uint64_t) * nnodes);
	for (auto& block : mem_blocks)
	{
		for(int i = 0; i < nnodes; i++)
            if(block.num_pages_on_node[i]>=0)
			    pages[i] += block.num_pages_on_node[i];
	}
	return 0;
}
