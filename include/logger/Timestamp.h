#ifndef SHUVLOG_TIMESTAMP_H
#define SHUVLOG_TIMESTAMP_H

#include <chrono>

using namespace std::chrono;

std::tm fromTimePoint(time_point<system_clock> timestamp);
std::string formatTimestamp(
    time_point<system_clock> timestamp,
    bool showOnlyTime = false,
    bool showMilliseconds = true,
    bool forFilename = false
);

#endif //SHUVLOG_TIMESTAMP_H
