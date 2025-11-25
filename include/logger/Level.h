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

}

#endif //SHUVLOG_LEVEL_H
