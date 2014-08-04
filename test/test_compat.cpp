#include <iostream>
#include "gtest/gtest.h"

#include "framework/compat.h"

using namespace std;

class CompatTest : public ::testing::Test {
protected:
    CompatTest() {
        cp = Compat::get_instance();
    }

    Compat* cp;
};

TEST_F(CompatTest, ubuntu)
{
    bool catched = false;
    try {
        cp->compat_checking(CompatItem::UBUNTU);
    } catch(CompatCheckFailed e) {
        cout << "catch exception CompatCheckFailed" << endl
            << "what(): " << e.what() << endl;
        catched = true;
    }

    ASSERT_EQ(catched, false);
}
