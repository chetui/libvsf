#ifndef _UTILS_FUNC_OPTION_
#define _UTILS_FUNC_OPTION_

#include <set>
#include <map>
#include <string>
#include <initializer_list>
#include <stdexcept>

enum class Option : unsigned int { 
    OP_MISS_RATE, 
    OP_MEM_SAMPLE
};

class FuncOption 
{
public:
    static FuncOption* get_instance();
    void enable_option(std::initializer_list<Option> ops);
    void disable_option(std::initializer_list<Option> ops);
    void check_option(std::string func_name, std::initializer_list<Option> ops);

private:
    FuncOption();
    std::set<Option> options_;
    std::map<const Option, const std::string> option_str_ = {
        { Option::OP_MISS_RATE, "OP_MISS_RATE" },
        { Option::OP_MEM_SAMPLE, "OP_MEM_SAMPLE" }
    };
};

class OpNotEnable : public std::logic_error {
public:
    explicit OpNotEnable(const std::string &op_str, const std::string &s):
        std::logic_error(op_str + " is not enabled. It is needed by function: " + s + ". Please use VSF::init() to enable " + op_str + ".") 
    {}
};

#endif
