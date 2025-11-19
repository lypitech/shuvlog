#ifndef SHUVLOG_LEVEL_H
#define SHUVLOG_LEVEL_H

#include <cstdint>

namespace logger
{

enum class Level : uint8_t
{
    kDebug      = 0x00,
    kInfo       = 0x01,
    kWarning    = 0x02,
    kError      = 0x03,
    kCritical   = 0x04,
    kFatal      = 0x05
};

}

#endif //SHUVLOG_LEVEL_H
