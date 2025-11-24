#ifndef SHUVLOG_NDJSONFILESINK_H
#define SHUVLOG_NDJSONFILESINK_H

#include "logger/FileSink.h"

namespace logger
{

/**
 * @class   NdJsonFileSink
 *
 * This sink serializes log messages into unformatted NDJSON and writes
 * them to a @code .ndjson@endcode file.
 *
 * @see @class JsonFileSink
 *
 * @warning Registering multiple file sinks targeting the same output file
 *          will throw a @code DuplicateSink@endcode exception.
 */
class NdJsonFileSink : public FileSink
{
public:
    explicit NdJsonFileSink(
        const std::string& filepath,
        sink::Settings settings = sink::Settings()
    ) : FileSink(filepath, "NDJSON", ".ndjson", settings) {}

    void write(const Log& log) override;
    void writeHeader(
        const std::string &projectName,
        int argc,
        const char* argv[],
        const BuildInfo& buildInfo,
        const Settings& settings
    ) override;
    void flush() override;
    void close() override;
};

}

#endif //SHUVLOG_NDJSONFILESINK_H
