#include "Sink.h"

#include "logger/Exceptions/InvalidLevel.h"

namespace logger
{

Sink::Sink(sink::Settings settings)
    : _settings(settings)
    , _filterMode(sink::FilterMode::kAll)
    , _minimumLevel(Level::kDebug)
    , _levelMask(0xFFFF)
{}

Sink::Sink(
    sink::FilterMode filterMode,
    uint16_t levelMask,
    sink::Settings settings
)
    : _settings(settings)
    , _filterMode(filterMode)
    , _minimumLevel(Level::kDebug)
    , _levelMask(0xFFFF)
{
    switch (filterMode)
    {
        case sink::FilterMode::kMinimumLevel: {
            if (!isSingleLevel(levelMask)) {
                throw exception::InvalidLevel();
            }

            _minimumLevel = static_cast<Level>(levelMask);
            break;
        }

        case sink::FilterMode::kExplicit: {
            _levelMask = levelMask;
            break;
        }

        case sink::FilterMode::kAll: {
            break;
        }
    }
}

bool Sink::shouldLog(Level level) const
{
    switch (_filterMode)
    {
        case sink::FilterMode::kAll:
            return true;

        case sink::FilterMode::kMinimumLevel:
            return static_cast<uint16_t>(level) >=
                   static_cast<uint16_t>(_minimumLevel);

        case sink::FilterMode::kExplicit:
            return (static_cast<uint16_t>(level) & _levelMask) != 0;
    }
    return false;
}

bool Sink::isSingleLevel(uint16_t value)
{
    // A power of 2 has exactly one bit set
    // value & (value - 1) removes the rightmost bit
    // If result is 0 and value is non-zero, it's a single bit
    return value != 0 && (value & (value - 1)) == 0;
}

}
