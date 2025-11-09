#include "logger/Timestamp.h"

#include <chrono>
#include <iomanip>

std::tm fromTimePoint(time_point<system_clock> timestamp)
{
    std::tm tm{};
    const auto t = system_clock::to_time_t(timestamp);

    localtime_r(&t, &tm);
    return tm;
}

std::string formatTimestamp(time_point<system_clock> timestamp, bool forFilename)
{
    const std::tm tm = fromTimePoint(timestamp);
    std::ostringstream oss;

    if (forFilename) {
        oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
    } else {
        const auto ms = duration_cast<milliseconds>(timestamp.time_since_epoch()) % 1000;

        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
            << '.' << std::setw(3) << std::setfill('0') << ms.count();
    }
    return oss.str();
}
