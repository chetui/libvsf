#include <set>
#include <vector>
#include "gtest/gtest.h"

#include "sysinfo/host/static/node_dist.h"

class NodeDistTest: public ::testing::Test {
protected:
    NodeDistTest() {
        nd = NodeDist::get_instance();
    }

    NodeDist* nd;
};

void print_dist( std::vector<std::vector<int> >& dist)
{
    for(std::vector<std::vector<int> >::iterator i = dist.begin(); i != dist.end(); i++)
    {
        for(std::vector<int>::iterator ii = i->begin(); ii != i->end(); ii++)
            std::cout<<*ii<<" ";
        std::cout<<std::endl;
    }
}

TEST_F(NodeDistTest, sys_matrix)
{
    std::vector<std::vector<int> > res = nd->get_sys_node_dist();
    print_dist(res);
    ASSERT_EQ(res[0][1], 20);
}

TEST_F(NodeDistTest, sys_one)
{
    std::cout << nd->get_sys_node_dist(1, 0) << std::endl;
    ASSERT_EQ(nd->get_sys_node_dist(1, 0), 20);
}

TEST_F(NodeDistTest, test_matrix)
{
    //default
    MicroParam dp;
    std::vector<std::vector<int> > res = nd->get_test_node_dist(dp);
    print_dist(res);

    //custom
    MicroParam cp(".", 23, WORKLOADTYPE_RANDOM, 212);
    res = nd->get_test_node_dist(cp);
    print_dist(res);
    ASSERT_EQ(true, res[0][1] > 10);
}

TEST_F(NodeDistTest, test_one)
{
    MicroParam p(".", 23, WORKLOADTYPE_RANDOM, 212);
    std::cout << nd->get_test_node_dist(0, 1, p) << std::endl;
    ASSERT_EQ(true, nd->get_test_node_dist(0, 1, p) > 10);
}
