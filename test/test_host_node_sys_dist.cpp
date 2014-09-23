#include <set>
#include <vector>
#include "gtest/gtest.h"

#include "sysinfo/host/static/node_sys_dist.h"

class NodeSysDistTest: public ::testing::Test {
protected:
    NodeSysDistTest() {
        nd = NodeSysDist::get_instance();
    }

    NodeSysDist* nd;
};

void print_dist(const std::vector<std::vector<int> >& dist)
{
    for (const auto& col : dist)
    {
        for (const auto& row : col)
            std::cout << row << " ";
        std::cout << std::endl;
    }

}

TEST_F(NodeSysDistTest, matrix)
{
    std::vector<std::vector<int> > res = nd->get_node_sys_dist();
    print_dist(res);
    ASSERT_EQ(res[0][1], 20);
}

TEST_F(NodeSysDistTest, one)
{
    std::cout << nd->get_node_sys_dist(1, 0) << std::endl;
    ASSERT_EQ(nd->get_node_sys_dist(1, 0), 20);
}

