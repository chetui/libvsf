#include <unistd.h>
#include <iostream>
#include <set>
#include "gtest/gtest.h"

#include "sysinfo/vm/dynamic/vm_used_mem_size.h"

using namespace std;

class VmUsedMemSizeTest : public ::testing::Test {
protected:
    VmUsedMemSizeTest() {
        vm_used_mem_size = VmUsedMemSizeTest::get_instance();
    }

    VmUsedMemSizeTest* vm_used_mem_size;
};




TEST_F(VmUsedMemSizeTest, vm_used_mem_size_function)
{
    vm_used_mem_size->start();
	auto ret=vm_used_mem_size->used_mem_size();
	auto n=ret.size();
	ASSERT_EQ(n>0,true);
	cout<<"VM us"
	for(int i=0;i<n;i++)
		vm_used_mem_size->used_mem_size(i);
    vm_used_mem_size->stop();
}

