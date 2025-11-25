#include "logger/Sink.h"

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
    uint16_t levelSpec,
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
            if (!isSingleLevel(levelSpec)) {
                throw exception::InvalidLevel();
            }

            _minimumLevel = static_cast<Level>(levelSpec);
            break;
        }

        case sink::FilterMode::kExplicit: {
            _levelMask = levelSpec;
            break;
        }

        case sink::FilterMode::kAll: {
            break;
        }
    }
}

bool Sink::isSingleLevel(uint16_t value)
{
    // A power of 2 has exactly one bit set
    // value & (value - 1) removes the rightmost bit
    // If result is 0 and value is non-zero, it's a single bit
    return value != 0 && (value & (value - 1)) == 0;
}

}
