#ifndef _COMPAT_H_
#define _COMPAT_H_

#include <map>
#include <string>
#include <mutex>
#include "framework/exception.h"

enum class CompatItem : unsigned int { 
    UBUNTU,
    KERNEL_3_8_0_LATER
};


class Compat {
public:
    static Compat *get_instance();
    void compat_checking(CompatItem);

private:
    Compat();
    std::map<CompatItem, bool> check_res_;
    std::mutex check_res_mutex_;

    void do_compat_checking(CompatItem ci, bool (*check_func)(), std::string excep_str);

    static bool ubuntu_checking();
    static bool kernel_3_8_0_later_checking();

};

#endif
