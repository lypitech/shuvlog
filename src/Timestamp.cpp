#include "logger/Timestamp.h"

#include <chrono>

std::tm fromTimePoint(time_point<system_clock> timestamp)
{
    std::tm tm{};
    const auto t = system_clock::to_time_t(timestamp);

    localtime_r(&t, &tm);
    return tm;
}
