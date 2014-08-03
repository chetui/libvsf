#ifndef _UTILS_FUNC_OPTION_
#define _UTILS_FUNC_OPTION_

#include <set>
#include <map>
#include <string>
#include <initializer_list>
#include <stdexcept>
#include "sysinfo/define_func_option.h"

class OptionParamVal;

class FuncOption 
{
public:
    static FuncOption* get_instance();
    void enable_option(std::initializer_list<Option> ops);
    void disable_option(std::initializer_list<Option> ops);
    void check_option(Func fc);

private:
    FuncOption();
    std::set<Option> options_;
};

class OpNotEnable : public std::logic_error 
{
public:
    explicit OpNotEnable(const std::string &op_str, const std::string &s):
        std::logic_error(
op_str + " is not enabled. \
It is needed by function: " + s + ". \
Please use VSF::init() to enable " + op_str + "."
        ) 
    {}
};

class OptionParamVal 
{
public:
    OptionParamVal();
    OptionParamVal(const OptionParamVal &o);
    OptionParamVal(const std::string&);
    OptionParamVal(const char *);
    OptionParamVal(const char);
    OptionParamVal(const int);
    OptionParamVal(const double);
    ~OptionParamVal();
    OptionParamVal &operator=(const OptionParamVal&);
    OptionParamVal &operator=(const std::string&);
    OptionParamVal &operator=(const char *);
    OptionParamVal &operator=(const char);
    OptionParamVal &operator=(const int);
    OptionParamVal &operator=(const double);
    std::string get_string();
    char get_char();
    int get_int();
    double get_double();

private:
    enum {STR, CHAR, INT, DBL} type;
    union {
        std::string sval;
        char cval;
        int ival;
        double dval;
    };
    void copy_union(const OptionParamVal&);
};

#endif
