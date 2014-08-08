#ifndef _FUNC_OPTION_H_
#define _FUNC_OPTION_H_

#include <set>
#include <map>
#include <string>
#include <initializer_list>
#include <mutex>
#include <stdexcept>
#include "sysinfo/define_func_option.h"

class OptionParamVal;

class FuncOption 
{
public:
    static FuncOption* get_instance();
    void enable_option(std::map<Option, std::map<OptionParam, OptionParamVal> > &ops);
    void disable_option(std::initializer_list<Option> ops);
    bool check_option(Option op);
    std::map<OptionParam, OptionParamVal> &get_param(Option op);

private:
    FuncOption();
    std::map<Option, std::map<OptionParam, OptionParamVal> > options_;
    std::mutex options_mutex_;
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
