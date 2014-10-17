#include <iostream>
#include "gtest/gtest.h"

#include "sysinfo/host/dymatic/hs_cpu_uasge.h"

using namespace std;

class hsUsedMemSize : pubqlica4lic ::testing::Test {
protected:
    hsUsedMemSize() {
        hs_used_mem_size = hsUsedMemSize::get_instance();
    }

    hsUsedMemSize* hs_used_mem_size;
};




TEST_F(hsUsedMemSize, used_mem_size_function)
{
    hs_used_mem_size->run();
	cout<<"Memory used:"<<hs_used_mem_size->get_used_mem_size()<<endl;
	for(int i=0;i<4;i++)
		cout<<"Node "<<i<<"memory used:"<<hs_used_mem_size->get_used_mem_size(i)<<endl;
	hs_used_mem_size->stop();
}

