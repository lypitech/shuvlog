#include "logger/Logger.h"

namespace logger
{

Settings::Settings(
    const Level minimumLevel,
    const bool showTimestamp,
    const bool showSource,
    const bool coloredOutput,
    const size_t maxBatchSize,
    const int flushIntervalMs
)   : _minimumLevel(minimumLevel)
    , _showTimestamp(showTimestamp)
    , _showSource(showSource)
    , _coloredOutput(coloredOutput)
    , _maxBatchSize(maxBatchSize)
    , _flushIntervalMs(flushIntervalMs)
{}


void Settings::setMinimumLevel(const Level minimumLevel)
{
    _minimumLevel = minimumLevel;
}

void Settings::setShowTimestamp(const bool showTimestamp)
{
    _showTimestamp = showTimestamp;
}

void Settings::setShowSource(const bool showSource)
{
    _showSource = showSource;
}

void Settings::setColoredOutput(const bool colored)
{
    _coloredOutput = colored;
}

void Settings::setMaxBatchSize(const size_t maxBatchSize)
{
    _maxBatchSize = maxBatchSize;
}

void Settings::setFlushIntervalMs(const int flushIntervalMs)
{
    _flushIntervalMs = flushIntervalMs;
}

}
