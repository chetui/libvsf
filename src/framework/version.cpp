#include "framework/version.h"

const std::string& Version::name()
{
    static const std::string NAME = "libvsf";
    return NAME;
}

const std::string& Version::version()
{
    static const std::string VERSION = "0.1.0";
    return VERSION;
}
