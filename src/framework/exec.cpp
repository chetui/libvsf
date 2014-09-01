#include "framework/exec.h"


Exec::Exec()
{
    cpu_mig_ = CpuMig::get_instance();
}

Exec::~Exec()
{
}

Exec* Exec::get_instance()
{
    static Exec exec;
    return &exec;
}

void Exec::exec_mig()
{
    cpu_mig_->exec_cpu_mig();
}
