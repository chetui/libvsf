#ifndef _COMPAT_H_
#define _COMPAT_H_

#include <map>
#include <string>
#include <stdexcept>

enum class CompatItem : unsigned int { 
    UBUNTU,
    KERNEL_3_8_0_LATER
};

class CompatCheckFailed : public std::logic_error 
{
public:
    explicit CompatCheckFailed(const std::string &str):
        std::logic_error(
            "Compatibility checking is failed. " + str
        ) 
    {}
};


class Compat {
public:
    static Compat *get_instance();
    void compat_checking(CompatItem);

private:
    Compat();
    std::map<CompatItem, bool> check_res_;

    void do_compat_checking(CompatItem ci, bool (*check_func)(), std::string excep_str);

    static bool ubuntu_checking();
    static bool kernel_3_8_0_later_checking();

};

#endif
