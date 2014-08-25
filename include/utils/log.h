#ifndef UMS_UTILS_LOG_H_
#define UMS_UTILS_LOG_H_

#include <syslog.h>
#include <string>
#include <sstream>

#define LOG(x) LogStream(x)
#define LDEBUG LogStream(LogLevel::debug)

#define LERR LogStream(LogLevel::err) \
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
    void puts(LogLevel priority, const std::string& msg);
    static const std::string& indent();

    char* str_buf_;
    void** trace_buf_;
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
    explicit inline LogStream(LogLevel level = LogLevel::info):
        level(level) {}
    inline ~LogStream();
private:
    LogLevel level;
};

LogStream::~LogStream()
{
    Log::get()->puts(level, this->str());
}

#endif  // UMS_UTILS_LOG_H_
