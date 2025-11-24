#ifndef SHUVLOG_LEVEL_H
#define SHUVLOG_LEVEL_H

#include <cstdint>

namespace logger
{

enum class Level : uint8_t
{
    kDebug      = 0x00,
    kTraceR3    = 0x01,
    kTraceR2    = 0x02,
    kTraceR1    = 0x03,
    kInfo       = 0x04,
    kWarning    = 0x05,
    kError      = 0x06,
    kCritical   = 0x07,
    kFatal      = 0x08
};

}

#endif //SHUVLOG_LEVEL_H
