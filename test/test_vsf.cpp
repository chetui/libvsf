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
