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
    vsf->set_func_options({Option::OP_HW_NODE_CORE_HPTHREAD});
    vsf->exec_init();
    ASSERT_EQ(vsf->node_num(), 2);
}
