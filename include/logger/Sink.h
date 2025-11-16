#ifndef SHOVOLOGGER_SINK_H
#define SHOVOLOGGER_SINK_H

#include <string>

#include "BuildInfo.h"
#include "Log.h"
#include "Settings.h"

namespace logger
{

namespace sink
{

    struct Settings
    {
        bool showTimestamp = true;
        bool showOnlyTime = true;
        bool showMilliseconds = true;

        bool showThreadInfo = true;
        bool showThreadId = true;

        bool showSource = true;
        bool showLineNumber = true;
        bool showColumnNumber = true;
    };

}

class Sink
{
public:
    explicit Sink(sink::Settings settings)
        : _settings(settings) {}
    virtual ~Sink() = default;

    virtual void write(const Log& log) = 0;
    virtual void writeHeader(
        const std::string& projectName,
        int argc,
        const char* argv[],
        const BuildInfo& buildInfo,
        const Settings& settings
    ) = 0;
    virtual void flush() = 0;
    virtual void close() = 0;

protected:
    sink::Settings _settings;
};

}

#endif //SHOVOLOGGER_SINK_H
