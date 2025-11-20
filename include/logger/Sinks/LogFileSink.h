#ifndef SHUVLOG_LOGFILESINK_H
#define SHUVLOG_LOGFILESINK_H

#include "logger/FileSink.h"
#include "logger/Sink.h"

namespace logger
{

class LogFileSink : public FileSink
{
public:
    explicit LogFileSink(
        const std::string& filepath,
        sink::Settings settings = sink::Settings()
    ) : FileSink(filepath, ".log", settings) {}

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

#endif //SHUVLOG_LOGFILESINK_H
