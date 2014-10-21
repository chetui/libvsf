#include <iostream>
#include "gtest/gtest.h"

#include "sysinfo/host/static/total_mem_size.h"

using namespace std;

class TotalMemSizeTest : public ::testing::Test {
protected:
    TotalMemSizeTest() {
        instance = totalMemSize::get_instance();
    }

    totalMemSize* instance;
};




TEST_F(TotalMemSizeTest, total_mem_size_function)
{
    long long ret=instance->get_total_mem_size();
	ASSERT_EQ(ret>=0,true);
	cout<<"total memory:"<<ret<<endl;
	for(int i=0;i<4;i++)
	{
		ret=instance->get_total_mem_size(i);
		ASSERT_EQ(ret>=0,true);
		cout<<"node "<<i<<" memory:"<<ret<<endl;
	}
}

