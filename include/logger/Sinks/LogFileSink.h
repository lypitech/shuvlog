#ifndef SHOVOLOGGER_LOGFILESINK_H
#define SHOVOLOGGER_LOGFILESINK_H

#include <fstream>

#include "logger/Sink.h"

namespace logger
{

class LogFileSink : public Sink
{
public:
    explicit LogFileSink(
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

#endif //SHOVOLOGGER_LOGFILESINK_H
