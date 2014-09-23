#include <set>
#include <vector>
#include "gtest/gtest.h"

#include "sysinfo/host/static/node_test_dist.h"

class NodeTestDistTest: public ::testing::Test {
protected:
    NodeTestDistTest() {
        nd = NodeTestDist::get_instance();
    }

    NodeTestDist* nd;
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

TEST_F(NodeTestDistTest, matrix)
{
    //default
    MicroParam dp;
    std::vector<std::vector<int> > res = nd->get_node_test_dist(dp);
    print_dist(res);

    //custom
    MicroParam cp(".", 23, WORKLOADTYPE_RANDOM, 212);
    res = nd->get_node_test_dist(cp);
    print_dist(res);
    ASSERT_EQ(true, res[0][1] > 10);
}

TEST_F(NodeTestDistTest, one)
{
    MicroParam p(".", 23, WORKLOADTYPE_RANDOM, 212);
    std::cout << nd->get_node_test_dist(0, 1, p) << std::endl;
    ASSERT_EQ(true, nd->get_node_test_dist(0, 1, p) > 10);
}
