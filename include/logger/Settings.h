#ifndef SHUVLOG_SETTINGS_H
#define SHUVLOG_SETTINGS_H

#include "Level.h"

namespace logger
{

class Settings final
{
public:
    explicit Settings(
        Level minimumLevel = Level::kInfo,
        size_t maxBatchSize = 64,
        int flushIntervalMs = 250
    )   : _minimumLevel(minimumLevel)
        , _maxBatchSize(maxBatchSize)
        , _flushIntervalMs(flushIntervalMs)
    {}

    [[nodiscard]] Level getMinimumLevel() const { return _minimumLevel; }
    [[nodiscard]] size_t getMaxBatchSize() const { return _maxBatchSize; }
    [[nodiscard]] int getFlushIntervalMs() const { return _flushIntervalMs; }

    void setMinimumLevel(Level minimumLevel) { _minimumLevel = minimumLevel; }
    void setMaxBatchSize(size_t maxBatchSize) { _maxBatchSize = maxBatchSize; }
    void setFlushIntervalMs(int flushIntervalMs) { _flushIntervalMs = flushIntervalMs; }

private:
    Level _minimumLevel;
    size_t _maxBatchSize;
    int _flushIntervalMs;
};

}

#endif //SHUVLOG_SETTINGS_H
