#ifndef SHOVOLOGGER_TIMESTAMP_H
#define SHOVOLOGGER_TIMESTAMP_H

#include <chrono>

using namespace std::chrono;

std::tm fromTimePoint(time_point<system_clock> timestamp);

#endif //SHOVOLOGGER_TIMESTAMP_H
