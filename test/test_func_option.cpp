#include <iostream>
#include <map>
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
    map<Option, map<OptionParam, OptionParamVal> > ops = {
        { Option::OP_HS_NODE_CORE_HPTHREAD, { } },
        { Option::OP_VM_MEM_SAMPLE, { } }
    };

    fo->enable_option(ops);
    bool has = fo->check_option(Option::OP_HS_NODE_CORE_HPTHREAD);

    ASSERT_EQ(has, true);
}

//TEST_F(FuncOptionTest, have_option_dep) 
//{
//    map<Option, map<OptionParam, OptionParamVal> > ops = {
//        { Option::OP_VM_MEM_BINDINFO, { } }
//    };
//    fo->enable_option(ops);
//    bool has0 = fo->check_option(Option::OP_HS_NODE_CORE_HPTHREAD);
//    bool has1 = fo->check_option(Option::OP_VM_VNODE);
//
//    ASSERT_EQ(has0, true);
//    ASSERT_EQ(has1, true);
//}

TEST_F(FuncOptionTest, have_no_option) 
{
    map<Option, map<OptionParam, OptionParamVal> > ops = {
        { Option::OP_VM_MISS_RATE, { } },
        { Option::OP_HS_NODE_CORE_HPTHREAD, { } }
    };
    fo->enable_option(ops);
    fo->disable_option({Option::OP_HS_NODE_CORE_HPTHREAD});
    bool has = fo->check_option(Option::OP_HS_NODE_CORE_HPTHREAD);

    ASSERT_EQ(has, false);
}

