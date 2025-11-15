#ifndef SHOVOLOGGER_SINK_H
#define SHOVOLOGGER_SINK_H

#include <string>

#include "BuildInfo.h"
#include "Log.h"
#include "Settings.h"

namespace logger
{

class Sink
{
public:
    virtual ~Sink() = default;

    virtual void write(const Log& log, const Settings& settings) = 0;
    virtual void writeHeader(
        const std::string& projectName,
        int argc,
        const char* argv[],
        const BuildInfo& buildInfo,
        const Settings& settings
    ) = 0;
    virtual void flush() = 0;
    virtual void close() = 0;
};

}

#endif //SHOVOLOGGER_SINK_H
