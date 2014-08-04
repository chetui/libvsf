#include "iostream"
#include "gtest/gtest.h"

#include "vsf.h"

class VsfTest : public ::testing::Test {
protected:
    VsfTest() {
        vsf = Vsf::get_instance();
    }

    Vsf* vsf;
};

TEST_F(VsfTest, node_num)
{
    vsf->init({ 
        { Option::OP_HS_NODE_CORE_HPTHREAD, {} }
    });
    Host *host = vsf->init_host();
    
    ASSERT_EQ(host->node_num(), 2);
}

TEST_F(VsfTest, test_node_dist)
{
    vsf->init({ 
        { Option::OP_HS_TEST_NODE_DIST, 
            { 
                { OptionParam::PATH, "." },
                { OptionParam::SIZE_IN_MB, 20 },
                { OptionParam::WORKLOAD_TYPE, (char)MWT_RANDOM },
                { OptionParam::LOOP, 200 }
            }
         }
    });
    Host *host = vsf->init_host();
    
    std::cout << host->test_node_dist(0, 1) << std::endl;
}

TEST_F(VsfTest, sys_node_dist)
{
    vsf->init({ 
        { Option::OP_HS_SYS_NODE_DIST, { } }
    });
    Host *host = vsf->init_host();
    
    ASSERT_EQ(host->sys_node_dist(0, 1), 20);
    ASSERT_EQ(host->sys_node_dist(1, 1), 10);
}
