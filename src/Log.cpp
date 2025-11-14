#include "logger/Logger.h"
#include "logger/Thread.h"

Log::Log(
    std::string message,
    const logger::Level level,
    const std::source_location& loc
)
    : _message(std::move(message))
    , _level(level)
    , _location(loc)
    , _threadId(std::this_thread::get_id())
    , _threadName(logger::getThreadLabel())
    , _timestamp(std::chrono::system_clock::now())
{}
