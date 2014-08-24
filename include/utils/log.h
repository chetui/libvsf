#ifndef UMS_UTILS_LOG_H_
#define UMS_UTILS_LOG_H_

#include <syslog.h>
#include <string>
#include <sstream>

#define LOG(x) LogStream(x)
#define LDEBUG() LogStream(LogLevel::debug)

#ifdef NDEBUG
#define LERR() LogStream(LogLevel::err) \
    << "[Error] file : " << __FILE__  \
    << " : in function " << __func__  \
    << " at line " << __LINE__  \
    << " (Compiled on " << __DATE__  \
    << " at " << __TIME__ << ")\n" 
#else
#define LERR() LogStream(LogLevel::err)
#endif

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
 * @author zsy
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

    static int option;
    static int facility;
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
