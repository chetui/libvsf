#include <iostream>
#include "gtest/gtest.h"

#include "sysinfo/vm/static/vm_node_info.h"

using namespace std;

class VmNodeInfoTest : public ::testing::Test {
protected:
    VmNodeInfoTest() {
        //vm_node_info = vmNodeInfo::get_instance();
    }

    //vmNodeInfo* vm_node_info;
};




TEST_F(VmNodeInfoTest, vm_node_info_function)
{
	int pid;
	cout<<"Input the pid of vm:"<<endl;
	cin>>pid;
    vmNodeInfo *vmnode=vmNodeInfo::get_instance(pid);
	int n=vmnode->get_vnode_num();
	cout<<"pid: "<<pid<<" has "<<n<<" vnodes."<<endl;
	auto ret=vmnode->get_vnode_ids();
	for(auto e:ret)
	{
		cout<<"vnode id:"<<e<<endl;
		auto v=vmnode->get_vcpu_ids(e);
		cout<<"vcpus:";
		for(auto i:v)
		{
			cout<<i<<" ";
		}
		cout<<endl;
	}
}

