#include <iostream>
#include "gtest/gtest.h"

#include "sysinfo/host/static/node_size.h"

using namespace std;

class NodeSizeTest : public ::testing::Test {
protected:
    NodeSizeTest() {
        //vm_used_mem_size = NodeSizeTest::get_instance();
    }

    //NodeSizeTest* vm_used_mem_size;
};




TEST_F(NodeSizeTest, node_size_function)
{
    node_data nodes[4];
    int n=get_node_size_info(nodes,4);
    for(int i=0;i<n;i++)
    {
        cout<<"node["<<i<<"]:"<<nodes[i].MBs_total<<endl;
    }
}

