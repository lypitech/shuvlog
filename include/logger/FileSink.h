#ifndef SHUVLOG_FILESINK_H
#define SHUVLOG_FILESINK_H

#include <fstream>

#include "Sink.h"

namespace logger
{

class FileSink : public Sink
{
public:
    explicit FileSink(
        const std::string& filepath,
        const std::string& recommendedExtension,
        sink::Settings settings
    );

    std::string getAbsoluteFilepath() const { return _absoluteFilepath; }

protected:
    std::fstream _file;
    const std::string _absoluteFilepath;
};

}

#endif //SHUVLOG_FILESINK_H
