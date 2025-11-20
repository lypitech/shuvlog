#include "logger/Timestamp.h"

#include <chrono>
#include <iomanip>

#if defined(_WIN32)
#   define localtime_r(T,Tm) localtime_s(Tm,T)
#endif

std::tm fromTimePoint(const time_point<system_clock> timestamp)
{
    std::tm tm{};
    const auto t = system_clock::to_time_t(timestamp);

    localtime_r(&t, &tm);
    return tm;
}

std::string formatTimestamp(
    const time_point<system_clock> timestamp,
    bool showOnlyTime,
    bool showMilliseconds,
    const bool forFilename
)
{
    std::string out;
    out.reserve(32);

    const std::tm tm = fromTimePoint(timestamp);

    if (forFilename) {
        std::format_to(std::back_inserter(out),
            "{:04}-{:02}-{:02}_{:02}-{:02}-{:02}",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec
        );
        return out;
    }

    if (showOnlyTime) {
        std::format_to(std::back_inserter(out),
            "{:02}:{:02}:{:02}",
            tm.tm_hour, tm.tm_min, tm.tm_sec
        );
    } else {
        std::format_to(std::back_inserter(out),
            "{:04}-{:02}-{:02} {:02}:{:02}:{:02}",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec
        );
    }

    if (showMilliseconds) {
        const auto ms =
            duration_cast<milliseconds>(timestamp.time_since_epoch()) % 1000;

        std::format_to(std::back_inserter(out), ".{:03}", ms.count());
    }

    return out;
}
