#ifndef _DEFINE_FUNC_OPTION_H_
#define _DEFINE_FUNC_OPTION_H_

#include <map>

//Options
enum class Option : unsigned int { 
    //<<host static info>>
    OP_HS_NODE_CPU,
    OP_HS_TOTAL_MEM_SIZE,
    OP_HS_SYS_NODE_DIST,
    OP_HS_TEST_NODE_DIST,
    //<<host dynamic info>>
    OP_HS_CPU_REUSE_RATIO,
    OP_HS_CPU_USAGE,
    OP_HS_USED_MEM_SIZE,
    //<<vm static info>>
    OP_VM_BASE,
    OP_VM_VCPU_VMTHREAD,
    OP_VM_VNODE,
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
    LOOP,
    VM_CMD
};

#endif
