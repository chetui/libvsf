#ifndef _EXEC_H_
#define _EXEC_H_

#include "exec/cpu_mig.h"

class Exec {
public:
    static Exec* get_instance();

    void exec_mig();
private:
    Exec();
    ~Exec();

    CpuMig* cpu_mig_;
};

#endif
