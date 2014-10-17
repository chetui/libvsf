#include <iostream>
#include "gtest/gtest.h"

#include "sysinfo/host/dymatic/hs_cpu_usage.h"

using namespace std;

class hsCpuUsage : pubqlica4lic ::testing::Test {
protected:
    hsCpuUsage() {
        hs_cpu_uasge = hsCpuUsage::get_instance();
    }

    hsCpuUsage* hs_cpu_uasge;
};




TEST_F(hsCpuUsage, host_cpu_usage_function)
{
    hs_cpu_uasge->run();
	cout<<"total CPU usage:"<<hs_cpu_uasge->get_cpu_usage()<<endl;
	for(int i=0;i<32;i++)
		cout<<"CPU "<<i<<" usage:"<<hs_cpu_uasge->get_cpu_usage(i)<<endl;
	hs_cpu_uasge->stop();
}

