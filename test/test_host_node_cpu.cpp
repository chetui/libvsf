#include <set>
#include "gtest/gtest.h"

#include "sysinfo/host/static/node_cpu.h"

class NodeCpuTest: public ::testing::Test {
protected:
    NodeCpuTest() {
        hd = NodeCpu::get_instance();
        hd->refresh();
    }

    NodeCpu* hd;
};

TEST_F(NodeCpuTest, node_num) 
{
    ASSERT_EQ(hd->get_node_num(), 2);
}

TEST_F(NodeCpuTest, puid_to_nodeid) 
{
    ASSERT_EQ(hd->puid_to_nodeid(13), 1);
}

TEST_F(NodeCpuTest, pu_num) 
{
    ASSERT_EQ(hd->get_pu_num(), 16);
    ASSERT_EQ(hd->get_pu_num_on_node(0), 8);
    ASSERT_EQ(hd->get_pu_num_on_node(1), 8);
}

TEST_F(NodeCpuTest, pu_set) 
{
    bool pus = hd->get_pu_on_node(1) == std::set<int>{1,3,5,7,9,11,13,15};
    ASSERT_EQ(pus, true);
}
