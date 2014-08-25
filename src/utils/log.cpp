#include "utils/log.h"
#include <execinfo.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <iostream>
#include "framework/version.h"

Log::Log()
{
    openlog(indent().c_str(), option_, facility_);
    str_buf_ = new char[STR_BUF_SIZE];
    trace_buf_ = new void*[TRACE_BUF_SIZE];
} 

Log::~Log()
{
    closelog();
    delete[] str_buf_;
    delete[] trace_buf_;
}

const std::string& Log::indent()
{
    static const std::string INDENT = Version::name();
    return INDENT;
}

Log* Log::get()
{
    static Log log;
    return &log;
}

void Log::puts(LogLevel priority, const std::string& msg)
{
    std::ostringstream out_str;
    if (priority == LogLevel::err 
        || priority == LogLevel::crit
        || priority == LogLevel::alert
        || priority == LogLevel::emerg) {
        out_str << "\n";
    }
        out_str << msg;
    if (priority == LogLevel::err 
        || priority == LogLevel::crit
        || priority == LogLevel::alert
        || priority == LogLevel::emerg) {
        out_str << "\n" << RED << "[Errno Info]\n " << NC << strerror_r(errno, str_buf_, sizeof(char) * STR_BUF_SIZE) << "\n";

        size_t size;
        size = backtrace(trace_buf_, TRACE_BUF_SIZE);
        char **traces = backtrace_symbols(trace_buf_, size);
        out_str << RED << "[Funtion Call Trace]\n " << NC << "return " << size << " addresses:" << "\n";
        for (size_t i = 0; i < size; ++i)
            out_str << traces[i] << "\n";
        free(traces);

        out_str << RED << "[Exception Info]" << NC;
    }
    if (priority == LogLevel::debug) {
#ifdef DEBUG
        std::cout << RED << "[Debug] " << NC << out_str.str() << "\n";
#endif
    } else {
        syslog(static_cast<int>(priority), "%s", out_str.str().c_str());
    }
}
