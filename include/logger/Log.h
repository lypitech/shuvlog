#ifndef SHUVLOG_LOG_H
#define SHUVLOG_LOG_H

#include <thread>
#include <chrono>
#include <source_location>

#include "Level.h"

class Log final
{
public:
    Log(
        std::string message,
        logger::Level level,
        const std::source_location& loc
    );

    [[nodiscard]] std::string getMessage() const { return _message; }
    [[nodiscard]] logger::Level getLevel() const { return _level; }
    [[nodiscard]] std::source_location getLocation() const { return _location; }
    [[nodiscard]] std::thread::id getThreadId() const { return _threadId; }
    [[nodiscard]] std::string getThreadName() const { return _threadName; }
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
