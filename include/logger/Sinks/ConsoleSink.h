#ifndef SHOVOLOGGER_CONSOLESINK_H
#define SHOVOLOGGER_CONSOLESINK_H

#include "logger/Sink.h"

namespace logger
{

class ConsoleSink : public Sink
{
public:
    explicit ConsoleSink(sink::Settings settings = sink::Settings());

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
};

}

#endif //SHOVOLOGGER_CONSOLESINK_H
