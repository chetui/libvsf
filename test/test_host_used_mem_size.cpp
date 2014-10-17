#include <iostream>
#include "gtest/gtest.h"

#include "sysinfo/host/dymatic/hs_used_mem_size.h"

using namespace std;

class hsUsedMemSizeTest : public ::testing::Test {
protected:
    hsUsedMemSizeTest() {
        hs_used_mem_size = hsUsedMemSize::get_instance();
    }

    hsUsedMemSize* hs_used_mem_size;
};




TEST_F(hsUsedMemSizeTest, used_mem_size_function)
{
    hs_used_mem_size->start();
	cout<<"Memory used:"<<hs_used_mem_size->get_used_mem_size()<<endl;
	for(int i=0;i<4;i++)
		cout<<"Node "<<i<<"memory used:"<<hs_used_mem_size->get_used_mem_size(i)<<endl;
	hs_used_mem_size->stop();
}

