#ifndef SHUVLOG_LOG_H
#define SHUVLOG_LOG_H

#include <thread>
#include <chrono>
#include <source_location>

#include "../include/logger/Level.h"

/**
 * @class   Log
 * @brief   A single logging event entry containing message text, metadata,
 *          and context.
 *
 * This class is an immutable container that stores all relevant information
 * about one log event.
 * It contains:
 *   - The log message
 *   - The severity level (@code logger::Level@endcode)
 *   - Source code metadata such as file, line number, and function obtained from
 *     @code std::source_location@endcode
 *   - Thread information (ID and user-defined thread label)
 *   - A timestamp captured at construction time
 *
 * This class captures all contextual information during class construction.
 */
class Log final
{
public:
    Log(
        std::string message,
        logger::Level level,
        const std::source_location& loc
    );

    /// @return The log message
    [[nodiscard]] std::string getMessage() const { return _message; }

    /// @return The severity level associated with this log entry
    [[nodiscard]] logger::Level getLevel() const { return _level; }

    /// @return The source location where this log entry was generated
    [[nodiscard]] std::source_location getLocation() const { return _location; }

    /// @return The ID of the thread that produced this log entry
    [[nodiscard]] std::thread::id getThreadId() const { return _threadId; }

    /// @return The name of the thread that produced this log entry
    [[nodiscard]] std::string getThreadName() const { return _threadName; }

    /// @return The timestamp representing when this log entry was constructed
    [[nodiscard]] std::chrono::time_point<std::chrono::system_clock> getTimestamp() const { return _timestamp; }

private:
    std::string _message;
    logger::Level _level;
    std::source_location _location;
    std::thread::id _threadId;
    std::string _threadName;
    std::chrono::time_point<std::chrono::system_clock> _timestamp;
};

#endif //SHUVLOG_LOG_H
