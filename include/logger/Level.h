#ifndef SHUVLOG_LEVEL_H
#define SHUVLOG_LEVEL_H

#include <cstdint>

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

}

#endif //SHUVLOG_LEVEL_H
