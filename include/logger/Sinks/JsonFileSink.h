#ifndef SHUVLOG_JSONFILESINK_H
#define SHUVLOG_JSONFILESINK_H

#include "logger/FileSink.h"

namespace logger
{

class JsonFileSink : public FileSink
{
public:
    explicit JsonFileSink(
        const std::string& filepath,
        sink::Settings settings = sink::Settings()
    ) : FileSink(filepath, ".json", settings) {}

    void write(const Log& log) override;
    void writeHeader(
        const std::string& projectName,
        int argc,
        const char* argv[],
        const BuildInfo& buildInfo,
        const Settings& settings
    ) override;
    void flush() override;
    void close() override;
};

}

#endif //SHUVLOG_JSONFILESINK_H
