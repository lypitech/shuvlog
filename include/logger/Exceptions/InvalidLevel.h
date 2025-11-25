#ifndef SHUVLOG_INVALIDLEVEL_H
#define SHUVLOG_INVALIDLEVEL_H

#include "LoggerException.h"

namespace logger::exception
{

class InvalidLevel : public LoggerException
{
public:
    explicit InvalidLevel()
        : LoggerException(
            "FilterMode::kMinimumLevel requires a single level, "
            "not a bitwise OR combination."
        ) {}
};

}

#endif //SHUVLOG_INVALIDLEVEL_H