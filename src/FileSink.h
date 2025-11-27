#ifndef SHUVLOG_FILESINK_H
#define SHUVLOG_FILESINK_H

#include <fstream>

#include "Sink.h"

namespace logger
{

/**
 * @class   FileSink
 * @brief   Abstract base class for sinks that outputs logs to a file.
 *
 * FileSink manages by itself extension verification, file opening, and
 * the file stream. File sinks (such as Log, JSON, or NDJSON)
 * should derive from this class and implement their own formatting logic
 * via the Sink interface.
 *
 * By default, a FileSink won't open the filestream if output file doesn't
 * end with the recommended extension.
 */
class FileSink : public Sink
{
public:
    /**
     * @brief   Constructs a FileSink and opens the file stream.
     *
     * @param   filepath                The user-provided path to the output file.
     * @param   extensionName           The name of the extension (e.g. "JSON", "Log", "NDJSON")
     * @param   recommendedExtension    The preferred file extension for this
     *                                  sink type (e.g. ".log", ".json", ".ndjson").
     * @param   settings                Format and metadata display settings for the sink.
     */
    explicit FileSink(
        const std::string& filepath,
        const std::string& extensionName,
        const std::string& recommendedExtension,
        sink::Settings settings
    );

    /**
     * @brief Same as first FileSink constructor but with level filtering specification.
     * @see @class  FileSink
     * @see @class  Sink
     */
    explicit FileSink(
        const std::string& filepath,
        const std::string& extensionName,
        const std::string& recommendedExtension,
        sink::FilterMode filterMode,
        uint16_t levelMask,
        sink::Settings settings
    );

    /**
     * @return  The full absolute path to the opened output file used by the sink.
     */
    std::string getAbsoluteFilepath() const { return _absoluteFilepath; }

protected:
    std::fstream _file;
    const std::string _absoluteFilepath;
};

}

#endif //SHUVLOG_FILESINK_H
