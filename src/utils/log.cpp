#include "utils/log.h"
#include <execinfo.h>
#include <unistd.h>
#include "framework/version.h"

using namespace std;

int Log::option = LOG_PERROR;
int Log::facility = LOG_DAEMON;

Log::Log()
{
    openlog(indent().c_str(), option, facility);
} 

Log::~Log()
{
    closelog();
}

const string& Log::indent()
{
    static const string INDENT = Version::name();
    return INDENT;
}

Log* Log::get()
{
    static Log slog;
    return &slog;
}

void Log::puts(LogLevel priority, const std::string& msg)
{
    ostringstream out_str;
    if (priority == LogLevel::err 
        || priority == LogLevel::crit
        || priority == LogLevel::alert
        || priority == LogLevel::emerg)
    {
        void *func_trace[128];
        size_t size;
        size = backtrace(func_trace, 128);
        char **traces = backtrace_symbols(func_trace, size);
        out_str << "[backtrace] return " << size << " addresses:" << "\n";
        for (size_t i = 0; i < size; ++i)
            out_str << traces[i] << "\n";
        free(traces);
    }
    out_str << msg << "\n";
    syslog(static_cast<int>(priority), "%s", out_str.str().c_str());
}
