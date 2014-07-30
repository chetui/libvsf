#include <set>
#include <gtest/gtest.h>

#include "sysinfo/hardware.h"

class HardwareTest: public ::testing::Test {
protected:
    HardwareTest() {
        hd = Hardware::get_instance();
    }

    Hardware* hd;
};

TEST_F(HardwareTest, node_num) 
{
    ASSERT_EQ(hd->get_node_num(), 2);
}

TEST_F(HardwareTest, puid_to_nodeid) 
{
    ASSERT_EQ(hd->puid_to_nodeid(13), 1);
}

TEST_F(HardwareTest, pu_num) 
{
    ASSERT_EQ(hd->get_pu_num(), 16);
    ASSERT_EQ(hd->get_pu_num_on_node(0), 8);
    ASSERT_EQ(hd->get_pu_num_on_node(1), 8);
}

TEST_F(HardwareTest, pu_set) 
{
    bool pus = hd->get_pu_on_node(1) == std::set<int>{1,3,5,7,9,11,13,15};
    ASSERT_EQ(pus, true);
}

void print_dist( std::vector<std::vector<int> >& dist)
{
    for(std::vector<std::vector<int> >::iterator i = dist.begin(); i != dist.end(); i++)
    {
        for(std::vector<int>::iterator ii = i->begin(); ii != i->end(); ii++)
            std::cout<<*ii<<" ";
        std::cout<<std::endl;
    }
}

TEST_F(HardwareTest, node_dist_no_param)
{
    std::vector<std::vector<int> > res = hd->get_node_dist();
    print_dist(res);
}

TEST_F(HardwareTest, node_dist_default_param)
{
    MicroParam p;
    std::vector<std::vector<int> > res = hd->get_node_dist(p);
    print_dist(res);
}

TEST_F(HardwareTest, node_dist_custom_param)
{
    MicroParam p(".", 20, MicroType::SERIAL, 20);
    std::vector<std::vector<int> > res = hd->get_node_dist(p);
    print_dist(res);
}
