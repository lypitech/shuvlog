#ifndef SHUVLOG_DUPLICATESINK_H
#define SHUVLOG_DUPLICATESINK_H

#include "LoggerException.h"

namespace logger::exception
{

class DuplicateSink : public LoggerException
{
public:
    explicit DuplicateSink(const std::string& filepath)
        : LoggerException(std::format(
            "{}: Another Sink is already using that file/stream.",
            filepath
        )) {}
};

}

#endif //SHUVLOG_DUPLICATESINK_H
