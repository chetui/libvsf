#include<string>
#include<vector>
#include<atomic>
#include<mutex>

class vmNodeInfo
{
	int vnode_num;
	vector<vm_vnode_static_info> vnode_info;
	atomic<bool> init;
	mutex mtx;
    uint32_t pid;
	vmNodeInfo(uint32_t pid):vnode_num(-1),init(false),pid(pid){};
public:
	static vmNodeInfo * get_instance(uint32_t pid);
	
	int update();
	int get_vnode_num();
    vector<int> get_vnode_ids();
    vector<int> get_vcpu_ids(int vnode_id,int vcpu_num);
};