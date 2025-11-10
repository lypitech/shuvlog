#ifndef SHOVOLOGGER_LOG_H
#define SHOVOLOGGER_LOG_H

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
    [[nodiscard]] std::chrono::time_point<std::chrono::system_clock> getTimestamp() const { return _timestamp; }

private:
    std::string _message;
    logger::Level _level;
    std::source_location _location;
    std::chrono::time_point<std::chrono::system_clock> _timestamp;
};

#endif //SHOVOLOGGER_LOG_H
