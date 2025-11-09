#ifndef SHOVOLOGGER_TIMESTAMP_H
#define SHOVOLOGGER_TIMESTAMP_H

#include <chrono>

using namespace std::chrono;

std::tm fromTimePoint(time_point<system_clock> timestamp);
std::string formatTimestamp(time_point<system_clock> timestamp, bool forFilename = false);

#endif //SHOVOLOGGER_TIMESTAMP_H
