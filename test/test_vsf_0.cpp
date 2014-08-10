#include <iostream>
#include <set>
#include "gtest/gtest.h"

#include "vsf.h"

using namespace std;

class VsfTest : public ::testing::Test {
protected:
    VsfTest() {
        vsf = Vsf::get_instance();
    }

    Vsf* vsf;
};

TEST_F(VsfTest, init_twice)
{
    vsf->init({});
    bool catched = false;
    try {
        vsf->init({});
    } catch (FrameworkInitMoreThanTwice e)
    {
        cout << "catch exception FrameworkInitMoreThanTwice" << endl
            << "what(): " << e.what() << endl;
        catched = true;
    }

    ASSERT_EQ(catched, true);
}

