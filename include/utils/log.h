#ifndef UMS_UTILS_LOG_H_
#define UMS_UTILS_LOG_H_

#include <syslog.h>
#include <string>
#include <sstream>
#include <memory>

#define LOG(x) LogStream(x)
#define LDEBUG LogStream(LogLevel::debug, PutsFunc::puts_debug_without_syslog)
#define LEXCEPT LogStream(LogLevel::err, PutsFunc::puts_exception) \
    << "\e[0;31m[Error Location] \e[0m\n"\
    << "in file: " << __FILE__  << "\n" \
    << "in function: " << __func__  << "\n" \
    << "at line: " << __LINE__  << "\n" \
    << "(Compiled on " << __DATE__  \
    << " at " << __TIME__ << ")\n" \
    << "\e[0;31m[Program Msg]\n \e[0m"

enum class LogLevel: int {
    emerg  = LOG_EMERG,
    alert  = LOG_ALERT,
    crit   = LOG_CRIT,
    err    = LOG_ERR,
    warn   = LOG_WARNING,
    notice = LOG_NOTICE,
    info   = LOG_INFO,
    debug  = LOG_DEBUG
};

enum class PutsFunc: int {
    puts,
    puts_exception,
    puts_debug_without_syslog
};

/**
 * Wrapper for syslog.
 */
class Log {
    friend class LogStream;
private:
    Log(); 
    ~Log();
    static Log* get(); 
    /**
     * Using syslog priority
     */
    static const std::string& indent();
    void puts(LogLevel priority, const std::string& msg);
    void puts_exception(LogLevel priority, const std::string& msg);
    void puts_debug_without_syslog(LogLevel priority, const std::string& msg);

    std::unique_ptr<char[]> str_buf_;
    std::unique_ptr<void*[]> trace_buf_;
    static constexpr int option_ = LOG_PERROR;
    static constexpr int facility_ = LOG_DAEMON;
    static constexpr const int STR_BUF_SIZE = 10240;
    static constexpr const int TRACE_BUF_SIZE = 128;
    static constexpr const char *RED = "\e[0;31m";
    static constexpr const char *NC = "\e[0m";
};

/**
 * Stream like interface of the log subsystem.
 */
class LogStream: public std::ostringstream 
{
public:
    explicit inline LogStream(LogLevel level = LogLevel::info, PutsFunc puts_func = PutsFunc::puts):
        level(level), puts_func(puts_func) {}
    inline ~LogStream();
private:
    LogLevel level;
    PutsFunc puts_func;
};

LogStream::~LogStream()
{
    switch(puts_func) {
    case PutsFunc::puts:
        Log::get()->puts(level, this->str());
        break;
    case PutsFunc::puts_exception:
        Log::get()->puts_exception(level, this->str());
        break;
    case PutsFunc::puts_debug_without_syslog:
        Log::get()->puts_debug_without_syslog(level, this->str());
        break;
    default:
        break;
    }
}

#endif  // UMS_UTILS_LOG_H_
