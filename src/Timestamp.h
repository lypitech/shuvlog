#ifndef SHUVLOG_TIMESTAMP_H
#define SHUVLOG_TIMESTAMP_H

#include <chrono>

using namespace std::chrono;

/**
 * @brief   Converts a @code std::chrono::system_clock::time_point@endcode
 *          to a @code std::tm@endcode structure.
 *
 * @param   timestamp   A UTC time point
 * @return  A @code std::tm@endcode structure containing the decomposed
 *          date and time fields.
 */
std::tm fromTimePoint(time_point<system_clock> timestamp);

/**
 * @brief   Formats a time point into a human-readable timestamp string.
 *
 * This function is used by the Logger system to consistently format
 * timestamps according to user preferences.
 *
 * Output behavior:
 * - If @code showOnlyTime@endcode is true, the result includes only the time portion.
 * - If @code showMilliseconds@endcode is true, fractional second precision is appended.
 * - If @code forFilename@endcode is true, all characters are made safe for filesystem
 *   (e.g., replacing ':' with '-') to form valid filenames.
 *
 * @param   timestamp           The UTC time point
 * @param   showOnlyTime        Whether to include only the time component
 * @param   showMilliseconds    Whether to show milliseconds in the output
 * @param   forFilename         Whether to format the timestamp for filesystem
 *                              usage
 *
 * @return  A formatted timestamp string.
 */
std::string formatTimestamp(
    time_point<system_clock> timestamp,
    bool showOnlyTime = false,
    bool showMilliseconds = true,
    bool forFilename = false
);

#endif //SHUVLOG_TIMESTAMP_H
