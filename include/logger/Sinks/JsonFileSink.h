#ifndef SHOVOLOGGER_JSONFILESINK_H
#define SHOVOLOGGER_JSONFILESINK_H

#include <fstream>

#include "logger/Sink.h"

namespace logger
{

class JsonFileSink : public Sink
{
public:
    explicit JsonFileSink(const std::string& filepath);

    void write(const Log& log, const Settings& settings) override;
    void writeHeader(
        const std::string &projectName,
        int argc,
        const char *argv[],
        const Settings &settings
    ) override;
    void flush() override;
    void close() override;

private:
    std::fstream _file;
};

}

#endif //SHOVOLOGGER_JSONFILESINK_H
