#ifndef SHUVLOG_LEVEL_H
#define SHUVLOG_LEVEL_H

#include <string>
#include <cstdint>

#include "../../src/Colors.h"

namespace logger
{

enum class Level : uint16_t
{
    kDebug      = 1 << 0,
    kTraceR3    = 1 << 1,
    kTraceR2    = 1 << 2,
    kTraceR1    = 1 << 3,
    kInfo       = 1 << 4,
    kWarning    = 1 << 5,
    kError      = 1 << 6,
    kCritical   = 1 << 7,
    kFatal      = 1 << 8,
};

inline uint16_t operator|(Level a, Level b)
{
    return static_cast<uint16_t>(a) | static_cast<uint16_t>(b);
}

inline uint16_t operator|(uint16_t a, Level b)
{
    return a | static_cast<uint16_t>(b);
}

namespace level
{

    /**
     * @brief   Converts a level to a string.
     *
     * @param   level   Logging severity level
     * @return  Converted data (e.g., "INFO", "ERROR")
     */
    inline std::string to_string(Level level)
    {
        switch (level) {
            using enum Level;
            case kDebug:    return "DEBUG";
            case kTraceR3:  return "TRACE_R3";
            case kTraceR2:  return "TRACE_R2";
            case kTraceR1:  return "TRACE_R1";
            case kInfo:     return "INFO";
            case kWarning:  return "WARNING";
            case kError:    return "ERROR";
            case kCritical: return "CRITICAL";
            case kFatal:    return "FATAL";
            default:        return "UNKNOWN";
        }
    }

    inline std::string getColor(Level level)
    {
        switch (level) {
            using enum Level;
            case kDebug:    return COLOR_BRIGHT_BLUE;
            case kTraceR3:  return COLOR_BRIGHT_BLACK;
            case kTraceR2:  return COLOR_GREEN;
            case kTraceR1:  return COLOR_CYAN;
            case kWarning:  return COLOR_YELLOW;
            case kError:    return COLOR_RED;
            case kCritical: return COLOR_MAGENTA;
            case kFatal:    return BACKGROUND_RED;
            default: return "";
        }
    }

    inline std::vector<Level> getIndividualLevelsFromMask(uint16_t levelMask)
    {
        std::vector<Level> levels;
        levels.reserve(std::popcount(levelMask)); // std::popcount returns exact amount of bits in levelMask

        while (levelMask != 0) {
            uint16_t lowestBit = levelMask & -levelMask;

            levels.push_back(static_cast<Level>(lowestBit));
            levelMask ^= lowestBit; // clearing
        }

        return levels;
    }

}

}

#endif //SHUVLOG_LEVEL_H
