#include <iostream>
#include "gtest/gtest.h"

#include "utils/func_option.h"

using namespace std;

class FuncOptionTest: public ::testing::Test {
protected:
    FuncOptionTest() {
        fo = FuncOption::get_instance();
    }

    FuncOption *fo;
};

TEST_F(FuncOptionTest, have_option) 
{
    bool catched = false;
    fo->enable_option({Option::OP_HS_NODE_CORE_HPTHREAD, Option::OP_VM_MEM_SAMPLE});
    try {
        fo->check_option(Func::FC_HS_NODE_NUM_P_);
    } catch (OpNotEnable e) {
        cout << "catch exception OpNotEnable" << endl
            << "what(): " << e.what() << endl;
        catched = true;
    }

    ASSERT_EQ(catched, false);
}

TEST_F(FuncOptionTest, have_no_option) 
{
    bool catched = false;
    fo->enable_option({Option::OP_VM_MISS_RATE, Option::OP_HS_NODE_CORE_HPTHREAD});
    fo->disable_option({Option::OP_HS_NODE_CORE_HPTHREAD});
    try {
        fo->check_option(Func::FC_HS_NODE_NUM_P_);
    } catch (OpNotEnable e) {
        cout << "catch exception OpNotEnable" << endl
            << "what(): " << e.what() << endl;
        catched = true;
    }

    ASSERT_EQ(catched, true);
}

