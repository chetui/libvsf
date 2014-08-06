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

TEST_F(VsfTest, node_num_and_node_dist)
{
    vsf->init({ 
        { Option::OP_HS_NODE_CORE_HPTHREAD, {} },
        { Option::OP_HS_SYS_NODE_DIST, { } },
        { Option::OP_HS_TEST_NODE_DIST, 
            { 
                { OptionParam::PATH, "." },
                { OptionParam::SIZE_IN_MB, 21 },
                { OptionParam::WORKLOAD_TYPE, WORKLOADTYPE_RANDOM },
                { OptionParam::LOOP, 210 }
            }
         }
    });
    Host *host = vsf->init_host();

    ASSERT_EQ(host->node_num(), 2);
    std::cout << host->test_node_dist(0, 1) << std::endl;
    ASSERT_EQ(true, host->test_node_dist(0, 1) > 10);
    std::cout << host->test_node_dist(0, 1, MicroParam(".", 23, WORKLOADTYPE_RANDOM, 230)) << std::endl;
    ASSERT_EQ(true, host->test_node_dist(0, 1, MicroParam(".", 23, WORKLOADTYPE_RANDOM, 230)) > 10);

    ASSERT_EQ(host->sys_node_dist(0, 1), 20);
    ASSERT_EQ(host->sys_node_dist(1, 1), 10);
}
