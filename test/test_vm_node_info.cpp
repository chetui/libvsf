#include <iostream>
#include "gtest/gtest.h"

#include "sysinfo/vm/static/vm_node_info.h"

using namespace std;

class VmNodeInfoTest : public ::testing::Test {
protected:
    VmNodeInfoTest() {
        //vm_used_mem_size = NodeSizeTest::get_instance();
    }

    //NodeSizeTest* vm_used_mem_size;
};




TEST_F(VmNodeInfoTest, vm_node_info_function)
{
	int pid;
	cout<<"Input the pid of vm:"<<endl;
	cin>>pid;
    vmNodeInfo vmnode(pid);
	int n=vmnode.get_vnode_num();
	cout<<"pid: "<<pid<<" "<<n<<"v nodes."<<endl;
	auto ret=vmnode.get_vnode_ids();
	for(auto e:ret)
	{
		cout<<"vnode id:"<<e<<endl;
		auto v=vmnode.get_vcpu_ids(e);
		cout<<"vcpus:"
		for(auto i:v)
		{
			cout<<i<<" ";
		}
		cout<<endl;
	}
}

