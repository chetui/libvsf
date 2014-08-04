#ifndef _DEFINE_FUNC_OPTION_H_
#define _DEFINE_FUNC_OPTION_H_

#include <map>
//#include <set>
//#include <string>

//Options
enum class Option : unsigned int { 
    //<<host static info>>
    OP_HS_NODE_CORE_HPTHREAD,
    OP_HS_TOTAL_MEM_SIZE,
    OP_HS_SYS_NODE_DIST,
    OP_HS_TEST_NODE_DIST,
    //<<host dynamic info>>
    OP_HS_CPU_REUSE_RATIO,
    OP_HS_CPU_USAGE,
    OP_HS_USED_MEM_SIZE,
    //<<vm static info>>
    OP_VM_VCPU_VMTHREAD,
    OP_VM_VNODE,
    OP_VM_TOTAL_MEM_SIZE,
    OP_VM_MEM_POLICY,
    //<<vm dynamic info>>
    OP_VM_CPU_BINDINFO,
    OP_VM_MEM_BINDINFO,
    OP_VM_CPU_USAGE,
    OP_VM_MISS_RATE,
    OP_VM_MEM_SAMPLE,
    OP_VM_USED_MEM_SIZE
};

extern std::multimap<const Option, const Option> option_dep;

enum class OptionParam : unsigned int {
    PATH,
    SIZE_IN_MB,
    WORKLOAD_TYPE,
    LOOP
};
//extern std::map<const Option, const std::string> option_str;
//
////Functions
//enum class Func : unsigned int { 
//    //<<host static info>>
//    FC_HS_NODE_NUM_P_,
//    FC_HS_NODE_IDS_P_,
//    FC_HS_NODE_ID_P_CORE_ID,
//    FC_HS_NODE_ID_P_HPTHREAD_ID,
//    FC_HS_CORE_NUM_P_,
//    FC_HS_CORE_NUM_P_NODE_ID,
//    FC_HS_CORE_IDS_P_,
//    FC_HS_CORE_IDS_P_NODE_ID,
//    FC_HS_CORE_ID_P_HPTHREAD_ID,
//    FC_HS_HPTHREAD_NUM_P_,
//    FC_HS_HPTHREAD_NUM_P_NODE_ID,
//    FC_HS_HPTHREAD_NUM_P_CORE_ID,
//    FC_HS_HPTHREAD_IDS_P_,
//    FC_HS_HPTHREAD_IDS_P_NODE_ID,
//    FC_HS_HPTHREAD_IDS_P_CORE_ID,
//    FC_HS_TOTAL_MEM_SIZE_P_,
//    FC_HS_TOTAL_MEM_SIZE_P_NODE_ID,
//    FC_HS_SYS_NODE_DIST_P_,
//    FC_HS_SYS_NODE_DIST_P_NODE_ID_0_NODE_ID_1,
//    FC_HS_TEST_NODE_DIST_P_,
//    FC_HS_TEST_NODE_DIST_P_NODE_ID_0_NODE_ID_1,
//    //<<host dynamic info>>
//    FC_HS_CPU_REUSE_RATIO_P_,
//    FC_HS_CPU_REUSE_RATIO_P_NODE_ID,
//    FC_HS_CPU_USAGE_P_,
//    FC_HS_CPU_USAGE_P_NODE_ID,
//    FC_HS_CPU_USAGE_P_CORE_ID,
//    FC_HS_CPU_USAGE_P_HPTHREAD_ID,
//    FC_HS_USED_MEM_SIZE_P_,
//    FC_HS_USED_MEM_SIZE_P_NODE_ID,
//    //<<VM static info>>
//    FC_VM_VM_ID_P_,
//    FC_VM_VMTHREAD_NUM_P_,
//    FC_VM_VMTHREAD_IDS_P_,
//    FC_VM_VCPU_NUM_P_,
//    FC_VM_VCPU_IDS_P_,
//    FC_VM_VCPU_IDS_P_VNODE_ID,
//    FC_VM_VNODE_NUM_P_,
//    FC_VM_VNODE_IDS_P_,
//    FC_VM_TOTAL_MEM_SIZE_P_,
//    FC_VM_MEM_POLICY_P_,
//    FC_VM_BINDINFO_MEM_NODE_IDS_P_,
//    //<<VM dynamic info>>
//    FC_VM_BINDINFO_HPTHREAD_IDS_P_,
//    FC_VM_BINDINFO_HPTHREAD_IDS_P_VCPU_ID,
//    FC_VM_BINDINFO_HPTHREAD_IDS_P_VMTHREAD_ID,
//    FC_VM_BINDINFO_MEM_NODE_ID_P_VNODE_ID,
//    FC_VM_CPU_USAGE_P_,
//    FC_VM_CPU_USAGE_P_VCPU_ID,
//    FC_VM_CPU_USAGE_P_VMTHREAD_ID,
//    FC_VM_MISS_RATE_P_MISS_RATE_TYPE,
//    FC_VM_MISS_RATE_P_MISS_RATE_TYPE_VCPU_ID,
//    FC_VM_MISS_RATE_P_MISS_RATE_TYPE_VMTHREAD_ID,
//    FC_VM_MEM_SAMPLE_P_,
//    FC_VM_USED_MEM_SIZE_P_,
//    FC_VM_USED_MEM_SIZE_P_NODE_ID
//};
//extern std::map<const Func, const std::string> func_str;
//
////Functions to Options
//extern std::map<Func, std::set<Option> > func_to_option;

#endif
