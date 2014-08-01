#include <utils/func_option.h>
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

void FuncOption::enable_option(initializer_list<Option> ops)
{
    options_.insert(ops.begin(), ops.end());

    return;
}

void FuncOption::disable_option(initializer_list<Option> ops)
{
    for (auto& op : ops) {
        options_.erase(op);
    }

    return;
}

void FuncOption::check_option(Func fc)
{
    set<Option> ops = func_to_option_[fc];
    for (auto& op : ops)
    {
        if (options_.count(op) == 0)
        {
            void *func_trace[128];
            size_t size;
            size = backtrace(func_trace, 128);
            backtrace_symbols_fd(func_trace, size, STDERR_FILENO);

            throw OpNotEnable(option_str_[op], func_str_[fc]);
        }
    }

    return;
}

