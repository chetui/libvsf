#include <unistd.h>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include "vsf.h"

Vsf* framework = Vsf::get_instance();

//#INSERT_HERE

int main()
{
//    //set some flags ahead of time to refresh optional functions info when init_host(), init_vms(), update_info();
//    //if others functions are called without set corresponding flags, info would be collected immediately.
//    framework->init({
//        { Option::OP_HS_NODE_CPU, { } },
//        { Option::OP_HS_NODE_SYS_DIST, { } },
//        { Option::OP_HS_NODE_TEST_DIST, 
//            { 
//                { OptionParam::PATH, "." },
//                { OptionParam::SIZE_IN_MB, 20 },
//                { OptionParam::WORKLOAD_TYPE, WORKLOADTYPE_RANDOM },
//                { OptionParam::LOOP, 200 }
//            }
//        },
//        { Option::OP_VM_BASE,
//            {
//                { OptionParam::VM_CMD, "qemu-system-x86_64" },
//                { OptionParam::LOOP_INTERVAL, 3000 }//,
////                { OptionParam::CALLBACK, VmBaseCallback(vm_base_print_callback) }
//            }
//        },
//        { Option::OP_VM_CPU_USAGE,
//            {
//                { OptionParam::LOOP_INTERVAL, 3000 }//,
////                { OptionParam::CALLBACK, VmCpuUsageCallback(vm_cpu_usage_print_callback) }
//            }
//        },
//        { Option::OP_VM_CACHE_MISS,
//            {
//                { OptionParam::LOOP_INTERVAL, 2000 },
//                { OptionParam::SAMPLE_INTERVAL, 50000 }//,
////                { OptionParam::CALLBACK, VmCacheMissCallback(vm_cache_miss_print_callback) }
//            }
//        }
//    });
//
//    //refresh <<Optional Host Static Info>>
//    Host *host = framework->init_host();
//    
//    while(true) {
//
//        //refresh <<Optional VM Static Info>>
//        //and start threads of <<Optional VM Dynamic Info>>
//        std::set<VM> vms = framework->init_vms(host);
//
//        //your scheduler algorithm
//        myscheduler(host, vms);
//
//        framework->exec_mig();
//
//        sleep(1); 
//    }
//
    return 0;
}
