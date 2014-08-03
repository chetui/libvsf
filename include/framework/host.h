#ifndef _HOST_H_
#define _HOST_H_

#include "utils/func_option.h"
#include "sysinfo/host/static/node_core_hpthread.h"

class Host {
public:
    static Host *get_instance();

    int node_num();

private:
    Host();

    FuncOption* func_option_;
    NodeCoreHpthread* node_core_hpthread_;
};

#endif
