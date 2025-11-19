#ifndef SHUVLOG_JSONFILESINK_H
#define SHUVLOG_JSONFILESINK_H

#include <fstream>

#include "logger/Sink.h"

namespace logger
{

class JsonFileSink : public Sink
{
public:
    explicit JsonFileSink(
        const std::string& filepath,
        sink::Settings settings = sink::Settings()
    );

    void write(const Log& log) override;
    void writeHeader(
        const std::string &projectName,
        int argc,
        const char *argv[],
        const BuildInfo& buildInfo,
        const Settings& settings
    ) override;
    void flush() override;
    void close() override;

private:
    std::fstream _file;
};

}

#endif //SHUVLOG_JSONFILESINK_H
