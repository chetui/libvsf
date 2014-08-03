#include <set>
#include "gtest/gtest.h"

#include "sysinfo/host/static/node_core_hpthread.h"

class NodeCoreHpthreadTest: public ::testing::Test {
protected:
    NodeCoreHpthreadTest() {
        hd = NodeCoreHpthread::get_instance();
    }

    NodeCoreHpthread* hd;
};

TEST_F(NodeCoreHpthreadTest, node_num) 
{
    ASSERT_EQ(hd->get_node_num(), 2);
}

TEST_F(NodeCoreHpthreadTest, puid_to_nodeid) 
{
    ASSERT_EQ(hd->puid_to_nodeid(13), 1);
}

TEST_F(NodeCoreHpthreadTest, pu_num) 
{
    ASSERT_EQ(hd->get_pu_num(), 16);
    ASSERT_EQ(hd->get_pu_num_on_node(0), 8);
    ASSERT_EQ(hd->get_pu_num_on_node(1), 8);
}

TEST_F(NodeCoreHpthreadTest, pu_set) 
{
    bool pus = hd->get_pu_on_node(1) == std::set<int>{1,3,5,7,9,11,13,15};
    ASSERT_EQ(pus, true);
}
