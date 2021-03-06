#ifndef _FUNC_OPTION_H_
#define _FUNC_OPTION_H_

#include <set>
#include <map>
#include <string>
#include <initializer_list>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include "framework/define_func_option.h"

class OptionParamVal;

class FuncOption 
{
public:
    static FuncOption* get_instance();
    void enable_option(const std::map<Option, std::map<OptionParam, OptionParamVal> > &ops);
    void disable_option(std::initializer_list<Option> ops);
    bool check_option(Option op);
    std::map<OptionParam, OptionParamVal> &get_param(Option op);

private:
    FuncOption();
    ~FuncOption();
    std::map<Option, std::map<OptionParam, OptionParamVal> > options_;
    std::shared_timed_mutex options_mutex_;
};

class OptionParamVal 
{
    friend class FuncOption;
public:
    OptionParamVal();
    OptionParamVal(const OptionParamVal &o);
    OptionParamVal(const std::string&);
    OptionParamVal(const char *);
    OptionParamVal(const char);
    OptionParamVal(const int);
    OptionParamVal(const double);
    OptionParamVal(void *const);
    ~OptionParamVal();
    OptionParamVal &operator=(const OptionParamVal&);
    OptionParamVal &operator=(const std::string&);
    OptionParamVal &operator=(const char *);
    OptionParamVal &operator=(const char);
    OptionParamVal &operator=(const int);
    OptionParamVal &operator=(const double);
    OptionParamVal &operator=(void *const);
    std::string get_string();
    char get_char();
    int get_int();
    double get_double();
    void* get_pointer();

private:
    enum {STR, CHAR, INT, DBL, PTR} type;
    union {
        std::string sval;
        char cval;
        int ival;
        double dval;
        void* pval;
    };
    void copy_union(const OptionParamVal&);
};

#endif
