#include <gtest/gtest.h>

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
    vsf->init({Option::OP_NODE_CORE_HPTHREAD});
    ASSERT_EQ(vsf->node_num(), 2);
}
