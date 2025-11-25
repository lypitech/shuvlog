#include "logger/Sink.h"

namespace logger
{

bool Sink::isSingleLevel(uint16_t value)
{
    // A power of 2 has exactly one bit set
    // value & (value - 1) removes the rightmost bit
    // If result is 0 and value is non-zero, it's a single bit
    return value != 0 && (value & (value - 1)) == 0;
}

}
