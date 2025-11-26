#ifndef SHUVLOG_JSONFILESINK_H
#define SHUVLOG_JSONFILESINK_H

#include "logger/FileSink.h"

namespace logger
{

/**
 * @class   JsonFileSink
 *
 * This sink serializes log messages into unformatted JSON and writes
 * them to a @code .json@endcode file.
 *
 * Each log entry is written as a JSON object, allowing automated processing,
 * or integration with log analysis tools.
 *
 * @warning Registering multiple file sinks targeting the same output file
 *          will throw a @code DuplicateSink@endcode exception.
 */
class JsonFileSink : public FileSink
{
public:
    explicit JsonFileSink(
        const std::string& filepath,
        sink::Settings settings = sink::Settings()
    );

    explicit JsonFileSink(
        const std::string& filepath,
        sink::FilterMode filterMode,
        uint16_t levelMask,
        sink::Settings settings = sink::Settings()
    );

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
