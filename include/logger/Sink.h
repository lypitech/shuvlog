#ifndef SHUVLOG_SINK_H
#define SHUVLOG_SINK_H

#include <string>

#include "BuildInfo.h"
#include "Log.h"
#include "Settings.h"

namespace logger
{

namespace sink
{

    /**
     * @struct  Settings
     * @brief   Configures how log messages should be formatted by the sink.
     *
     * These options control if timestamps, thread details, and source
     * information should appear in the rendered log output.
     *
     * @warning Sinks can decide if they take into account those settings.
     */
    struct Settings
    {
        bool showTimestamp = true;
        bool showOnlyTime = false;
        bool showMilliseconds = true;

        bool showThreadInfo = true;
        bool showThreadId = true;

        bool showSource = true;
        bool showLineNumber = true;
        bool showColumnNumber = true;
    };

    /**
     * @enum    FilterMode
     * @brief   Determines how the sink filters log levels.
     */
    enum class FilterMode
    {
        kAll,           ///< Accept all levels
        kMinimumLevel,  ///< Accept logs at or above a minimum level
        kExplicit,      ///< Accept only specified levels
    };

}

/**
 * @class   Sink
 * @brief   Abstract base class for all log sinks.
 *
 * A Sink defines the behavior for writing log messages to an output
 * destination, just like throwing water (logs) in a real-life sink.
 * Implementations must override the virtual methods to perform the
 * actual output operations.
 */
class Sink
{
public:
    /**
     * @brief   Constructs a sink with formatting settings only.
     * @param   settings    Settings struct describing how log data should be formatted
     */
    explicit Sink(sink::Settings settings);

    /**
     * @brief   Constructs a sink with formatting and filtering settings.
     *
     * @param   filterMode  Filtering mode to use
     * @param   levelSpec   Level specification (meaning depends on mode):
     *                      - kMinimumLevel: Must be a single level
     *                      - kExplicit: Bitwise OR of desired levels
     *                      - kAll: Parameter ignored
     * @param   settings    Settings struct describing how log data should be formatted
     *
     * @throws  exception::InvalidLevel if kMinimumLevel mode receives OR'd levels
     */
    Sink(
        sink::FilterMode filterMode,
        uint16_t levelSpec,
        sink::Settings settings
    );

    virtual ~Sink() = default;

    /**
     * @brief   Writes a log entry to the sink.
     * @param   log The log entry to be written
     */
    virtual void write(const Log& log) = 0;

    /**
     * @brief   Writes an initialization header to the sink.
     *
     * This function is called once at startup to keep a trace of project
     * metadata, how the project has been built, the exact command used
     * and global logging settings.
     *
     * @param   projectName Name of the project that uses the Logger
     * @param   argc        Number of command-line arguments (given in main)
     * @param   argv        Array of command-line arguments (given in main)
     * @param   buildInfo   Build metadata
     * @param   settings    Global Logger settings
     */
    virtual void writeHeader(
        const std::string& projectName,
        int argc,
        const char* argv[],
        const BuildInfo& buildInfo,
        const Settings& settings
    ) = 0;

    /**
     * @brief   Ensures that all buffered logs are written to the output.
     */
    virtual void flush() = 0;

    /**
     * @brief   Closes the sink.
     *
     * After this call the sink should not accept further writes.
     */
    virtual void close() = 0;

    /**
     * @brief   Checks if a log level should be processed by this sink.
     * @param   level The level to check
     * @return  true if the level passes the current filter
     */
    [[nodiscard]] bool shouldLog(Level level) const;

protected:
    sink::Settings _settings;
    sink::FilterMode _filterMode;
    Level _minimumLevel;
    uint16_t _levelMask;

private:
    /**
     * @brief   Checks if a value has exactly one bit set (isn't multiple levels at once).
     * @param   value   The value to check
     * @return  @code true@endcode if exactly one bit is set
     */
    static bool isSingleLevel(uint16_t value);
};

}

#endif //SHUVLOG_SINK_H
