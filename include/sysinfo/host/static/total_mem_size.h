#include<atomic>

#include<mutex>
#include"sysinfo/host/static/node_size.h"

struct phy_node
{
	int node_id;
	long long mem_size;
	phy_node(){}
	phy_node(node_id,mem_size):node_id(node_id),mem_size(mem_size);
}

class totalMemSize
{
	vector<phy_node> node_info;
	long long total_mem;
	atomic<bool> init;
	mutex mtx;
	totalMemSize():init(false){}
public:
	totalMemSize*¡¡get_instance();
	int get_phy_node_mem();
	void update();
	long long get_total_mem_size();
	long long get_total_mem_size(int node_id);
}
