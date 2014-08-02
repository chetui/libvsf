#ifndef _SYSINFO_DEFINE_FUNC_OPTION_
#define _SYSINFO_DEFINE_FUNC_OPTION_

#include <map>
#include <set>
#include <string>

//Options
enum class Option : unsigned int { 
    //<<hardware static info>>
    OP_HW_NODE_CORE_HPTHREAD,
    OP_HW_TOTAL_MEM_SIZE,
    OP_HW_SYS_NODE_DIST,
    OP_HW_TEST_NODE_DIST,
    //<<hardware dynamic info>>
    OP_HW_CPU_REUSE_RATIO,
    OP_HW_CPU_USAGE,
    OP_HW_USED_MEM_SIZE,
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
extern std::map<const Option, const std::string> option_str;

//Functions
enum class Func : unsigned int { 
    //<<hardware static info>>
    FC_HW_NODE_NUM_P_,
    FC_HW_NODE_IDS_P_,
    FC_HW_NODE_ID_P_CORE_ID,
    FC_HW_NODE_ID_P_HPTHREAD_ID,
    FC_HW_CORE_NUM_P_,
    FC_HW_CORE_NUM_P_NODE_ID,
    FC_HW_CORE_IDS_P_,
    FC_HW_CORE_IDS_P_NODE_ID,
    FC_HW_CORE_ID_P_HPTHREAD_ID,
    FC_HW_HPTHREAD_NUM_P_,
    FC_HW_HPTHREAD_NUM_P_NODE_ID,
    FC_HW_HPTHREAD_NUM_P_CORE_ID,
    FC_HW_HPTHREAD_IDS_P_,
    FC_HW_HPTHREAD_IDS_P_NODE_ID,
    FC_HW_HPTHREAD_IDS_P_CORE_ID,
    FC_HW_TOTAL_MEM_SIZE_P_,
    FC_HW_TOTAL_MEM_SIZE_P_NODE_ID,
    FC_HW_SYS_NODE_DIST_P_,
    FC_HW_SYS_NODE_DIST_P_NODE_ID_0_NODE_ID_1,
    FC_HW_TEST_NODE_DIST_P_,
    FC_HW_TEST_NODE_DIST_P_NODE_ID_0_NODE_ID_1,
    //<<hardware dynamic info>>
    FC_HW_CPU_REUSE_RATIO_P_,
    FC_HW_CPU_REUSE_RATIO_P_NODE_ID,
    FC_HW_CPU_USAGE_P_,
    FC_HW_CPU_USAGE_P_NODE_ID,
    FC_HW_CPU_USAGE_P_CORE_ID,
    FC_HW_CPU_USAGE_P_HPTHREAD_ID,
    FC_HW_USED_MEM_SIZE_P_,
    FC_HW_USED_MEM_SIZE_P_NODE_ID,
    //<<VM static info>>
    FC_VM_VM_ID_P_,
    FC_VM_VMTHREAD_NUM_P_,
    FC_VM_VMTHREAD_IDS_P_,
    FC_VM_VCPU_NUM_P_,
    FC_VM_VCPU_IDS_P_,
    FC_VM_VCPU_IDS_P_VNODE_ID,
    FC_VM_VNODE_NUM_P_,
    FC_VM_VNODE_IDS_P_,
    FC_VM_TOTAL_MEM_SIZE_P_,
    FC_VM_MEM_POLICY_P_,
    FC_VM_BINDINFO_MEM_NODE_IDS_P_,
    //<<VM dynamic info>>
    FC_VM_BINDINFO_HPTHREAD_IDS_P_,
    FC_VM_BINDINFO_HPTHREAD_IDS_P_VCPU_ID,
    FC_VM_BINDINFO_HPTHREAD_IDS_P_VMTHREAD_ID,
    FC_VM_BINDINFO_MEM_NODE_ID_P_VNODE_ID,
    FC_VM_CPU_USAGE_P_,
    FC_VM_CPU_USAGE_P_VCPU_ID,
    FC_VM_CPU_USAGE_P_VMTHREAD_ID,
    FC_VM_MISS_RATE_P_MISS_RATE_TYPE,
    FC_VM_MISS_RATE_P_MISS_RATE_TYPE_VCPU_ID,
    FC_VM_MISS_RATE_P_MISS_RATE_TYPE_VMTHREAD_ID,
    FC_VM_MEM_SAMPLE_P_,
    FC_VM_USED_MEM_SIZE_P_,
    FC_VM_USED_MEM_SIZE_P_NODE_ID
};
extern std::map<const Func, const std::string> func_str;

//Functions to Options
extern std::map<Func, std::set<Option> > func_to_option;

#endif
