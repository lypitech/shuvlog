#ifndef SHUVLOG_LOGFILESINK_H
#define SHUVLOG_LOGFILESINK_H

#include "logger/FileSink.h"

namespace logger
{

/**
 * @class   LogFileSink
 *
 * This sink writes human-readable log messages to a @code .log@endcode file.
 * Entries are formatted for readability and useful for debugging.
 *
 * Each log event is appended as a fully formatted line, including timestamp,
 * severity level, thread information, and message content, according to
 * sink's settings.
 *
 * @warning Registering multiple file sinks targeting the same output file
 *          will throw a @code DuplicateSink@endcode exception.
 */
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
