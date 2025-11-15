#ifndef SHOVOLOGGER_COULDNOTOPENFILE_H
#define SHOVOLOGGER_COULDNOTOPENFILE_H

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

#endif //SHOVOLOGGER_COULDNOTOPENFILE_H
