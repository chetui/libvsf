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

TEST_F(NodeDistTest, sys)
{
    std::vector<std::vector<int> > res = nd->get_sys_node_dist();
    print_dist(res);
}

TEST_F(NodeDistTest, test_default_param)
{
    MicroParam p;
    std::vector<std::vector<int> > res = nd->get_test_node_dist(p);
    print_dist(res);
}

TEST_F(NodeDistTest, test_custom_param)
{
    MicroParam p(".", 20, MWT_SERIAL, 20);
    std::vector<std::vector<int> > res = nd->get_test_node_dist(p);
    print_dist(res);
}
