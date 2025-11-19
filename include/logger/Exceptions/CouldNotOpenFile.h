#ifndef SHUVLOG_COULDNOTOPENFILE_H
#define SHUVLOG_COULDNOTOPENFILE_H

#include "LoggerException.h"

namespace logger::exception
{

class CouldNotOpenFile : public LoggerException
{
public:
    explicit CouldNotOpenFile(std::string filepath)
        : LoggerException(std::format(
            "{}: Could not open file.",
            filepath
        )) {}
};

}

#endif //SHUVLOG_COULDNOTOPENFILE_H
