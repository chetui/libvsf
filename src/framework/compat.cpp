#include "framework/compat.h"
#include <execinfo.h>
#include <unistd.h>

using namespace std;

Compat::Compat()
{
}

Compat* Compat::get_instance()
{
    static Compat compat;
    return &compat;
}

void Compat::compat_checking(CompatItem ci)
{
    switch(ci)
    {
    case CompatItem::UBUNTU:
        do_compat_checking(CompatItem::UBUNTU, ubuntu_checking, "Currently host machine is not Ubunut.");
        break;
    case CompatItem::KERNEL_3_8_0_LATER:
        do_compat_checking(CompatItem::KERNEL_3_8_0_LATER, kernel_3_8_0_later_checking, "The Linux kernel of host machine is not after 3.8.0 version.");
        break;
    }
}

void Compat::do_compat_checking(CompatItem ci, bool (*check_func)(), string excep_str)
{
    bool res;
    lock_guard<mutex> lock(check_res_mutex_);

    if(check_res_.count(ci) == 0) 
    {
        res = check_func();
        check_res_[ci] = res;
    }
    else
        res = check_res_[ci];
    if(!res)
    {
        void *func_trace[128];
        size_t size;
        size = backtrace(func_trace, 128);
        backtrace_symbols_fd(func_trace, size, STDERR_FILENO);
        throw CompatCheckFailed(excep_str);
    }

    return;
}

bool Compat::ubuntu_checking()
{
    return true;
}

bool Compat::kernel_3_8_0_later_checking()
{
    return true;
}
