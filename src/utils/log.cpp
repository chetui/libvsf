#include "utils/log.h"
#include <execinfo.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <iostream>
#include "framework/version.h"

Log::Log(): 
    str_buf_(new char[STR_BUF_SIZE]),
    trace_buf_(new void*[TRACE_BUF_SIZE])
{
    openlog(indent().c_str(), option_, facility_);
} 

Log::~Log()
{
    closelog();
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
    syslog(static_cast<int>(priority), "%s", msg.c_str());
}

void Log::puts_exception(LogLevel priority, const std::string& msg)
{
    priority = priority; //to suppresss unused warnning

    std::ostringstream out_str;
    out_str << "\n" << msg << "\n";
    out_str << RED << "[Errno Info]\n " << NC << strerror_r(errno, str_buf_.get(), sizeof(char) * STR_BUF_SIZE) << "\n";

    size_t size;
    size = backtrace(trace_buf_.get(), TRACE_BUF_SIZE);
    char **traces = backtrace_symbols(trace_buf_.get(), size);
    out_str << RED << "[Funtion Call Trace]\n " << NC << "return " << size << " addresses:" << "\n";
    for (size_t i = 0; i < size; ++i)
        out_str << traces[i] << "\n";
    free(traces);

    out_str << RED << "[Exception Info]" << NC;

    syslog(static_cast<int>(priority), "%s", out_str.str().c_str());
}

void Log::puts_debug_without_syslog(LogLevel priority, const std::string& msg)
{
    priority = priority; //to suppresss unused warnning
#ifdef DEBUG
    std::cout << RED << "[Debug] " << NC << msg << std::endl;
#else
    msg = msg; //to suppresss unused warnning
#endif
}
