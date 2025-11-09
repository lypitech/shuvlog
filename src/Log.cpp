#include "logger/Logger.h"

Log::Log(
    std::string message,
    const logger::Level level,
    const std::source_location& loc
)
    : _message(std::move(message))
    , _level(level)
    , _location(loc)
    , _timestamp(std::chrono::system_clock::now())
{}
