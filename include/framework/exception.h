#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <stdexcept>
#include "utils/log.h"

#define THROW(except, str) {\
    LERR() << str;\
    throw except(str);\
}\

#define LOGIC_ERROR(name, log_str) \
    class name : public std::logic_error \
    {\
    public:\
        explicit name(const std::string &str):\
            std::logic_error(\
                log_str + str\
            ) \
        {\
        }\
    };\

//vsf.h
LOGIC_ERROR(FrameworkInitMoreThanTwice, "Framework can not be initialized more than twice.")
//compat.h
LOGIC_ERROR(CompatCheckFailed, "Compatibility checking is failed. ")


#endif
