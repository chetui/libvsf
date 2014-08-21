#include "utils/func_option.h"
#include <execinfo.h>
#include <unistd.h>

using namespace std;

FuncOption::FuncOption()
{
}

FuncOption* FuncOption::get_instance()
{
    static FuncOption func_option;
    return &func_option;
}

void FuncOption::enable_option(std::map<Option, std::map<OptionParam, OptionParamVal> > &ops)
{
    unique_lock<shared_timed_mutex> lock(options_mutex_);

    //enable ops
    options_.insert(ops.begin(), ops.end());
    
//    //enable all the dependent options of ops
//    for (auto& op : ops)
//    {
//        auto cnt = option_dep.count(op.first);
//        auto iter = option_dep.find(op.first);
//        while(cnt) {
//            options_.insert({iter->second, map<OptionParam, OptionParamVal>()});//when adding dependent options, always take default parameters
//            ++iter;
//            --cnt;
//        }
//    }

    return;
}

void FuncOption::disable_option(initializer_list<Option> ops)
{
    unique_lock<shared_timed_mutex> lock(options_mutex_);

    for (auto& op : ops) {
        options_.erase(op);
    }

    return;
}

bool FuncOption::check_option(Option op)
{
    shared_lock<shared_timed_mutex> lock(options_mutex_);
    return options_.count(op) > 0;
}

std::map<OptionParam, OptionParamVal> &FuncOption::get_param(Option op)
{
    shared_lock<shared_timed_mutex> lock(options_mutex_);
    return options_[op];
}

OptionParamVal::OptionParamVal(): type(INT), ival{0} 
{ 
}

OptionParamVal::OptionParamVal(const OptionParamVal &o): type(o.type) 
{
    copy_union(o);
}

OptionParamVal::OptionParamVal(const string &s): type(STR), sval(s) 
{ 
}

OptionParamVal::OptionParamVal(const char *s): type(STR), sval(s) 
{ 
}

OptionParamVal::OptionParamVal(const char c): type(CHAR), cval(c) 
{ 
}

OptionParamVal::OptionParamVal(const int i): type(INT), ival(i) 
{ 
}

OptionParamVal::OptionParamVal(const double d): type(DBL), dval(d) 
{ 
}

OptionParamVal::~OptionParamVal()
{
    if (type == STR)
        sval.~string();
}

OptionParamVal& OptionParamVal::operator=(const OptionParamVal& o)
{
    if (type == STR && o.type != STR)
        sval.~string();
    if (type == STR && o.type == STR)
        sval = o.sval;
    else
        copy_union(o);
    type = o.type;

    return *this;
}

OptionParamVal& OptionParamVal::operator=(const string& s)
{
    if (type == STR)
        sval = s;
    else
        new(&sval) string(s);
    type = STR;

    return *this;

}

OptionParamVal& OptionParamVal::operator=(const char *s)
{
    if (type == STR)
        sval = s;
    else
        new(&sval) string(s);
    type = STR;

    return *this;

}

OptionParamVal& OptionParamVal::operator=(const char c)
{
    if (type == STR)
        sval.~string();
    cval = c;
    type = CHAR;

    return *this;
}

OptionParamVal& OptionParamVal::operator=(const int i)
{
    if (type == STR)
        sval.~string();
    ival = i;
    type = INT;

    return *this;
}

OptionParamVal& OptionParamVal::operator=(const double d)
{
    if (type == STR)
        sval.~string();
    dval = d;
    type = DBL;

    return *this;
}

std::string OptionParamVal::get_string()
{
    if (type == STR)
        return sval;
    else
        return "";
}

char OptionParamVal::get_char()
{
    if (type == CHAR)
        return cval;
    else
        return 0;
}

int OptionParamVal::get_int()
{
    if (type == INT)
        return ival;
    else
        return 0;
}

double OptionParamVal::get_double()
{
    if (type == DBL)
        return dval;
    else
        return 0.0;
}

void OptionParamVal::copy_union(const OptionParamVal &o)
{
    switch (o.type) {
    case STR: new(&sval) string(o.sval); break;
    case CHAR: cval = o.cval; break;
    case INT: ival = o.ival; break;
    case DBL: dval = o.dval; break;
    }

    return;
}


















