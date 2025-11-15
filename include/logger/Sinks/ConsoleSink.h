#ifndef SHOVOLOGGER_CONSOLESINK_H
#define SHOVOLOGGER_CONSOLESINK_H

#include "logger/Sink.h"

namespace logger
{

class ConsoleSink : public Sink
{
public:
    explicit ConsoleSink(Settings settings = Settings());

    void write(const Log& log, const Settings& settings) override;
    void writeHeader(
        const std::string &projectName,
        int argc,
        const char *argv[],
        const BuildInfo& buildInfo,
        const Settings &settings
    ) override;
    void flush() override;
    void close() override;
};

}

#endif //SHOVOLOGGER_CONSOLESINK_H
