#include <iostream>
#include <gtest/gtest.h>

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
    fo->enable_option({Option::OP_MISS_RATE, Option::OP_MEM_SAMPLE});
    try {
        fo->check_option("test_func", {Option::OP_MISS_RATE});
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
    fo->enable_option({Option::OP_MISS_RATE, Option::OP_MEM_SAMPLE});
    fo->disable_option({Option::OP_MEM_SAMPLE});
    try {
        fo->check_option("test_func", {Option::OP_MEM_SAMPLE});
    } catch (OpNotEnable e) {
        cout << "catch exception OpNotEnable" << endl
            << "what(): " << e.what() << endl;
        catched = true;
    }

    ASSERT_EQ(catched, true);
}

