#ifndef SHUVLOG_SETTINGS_H
#define SHUVLOG_SETTINGS_H

#include "Level.h"

namespace logger
{

/**
 * @class   Settings
 * @brief   Configuration options to customize logging behavior.
 */
class Settings final
{
public:

    /**
     * @brief Constructs a Settings object with optional default parameters.
     *
     * @param minimumLevel   The lowest log level that will be accepted.
     *                       Defaults to @code Level::kInfo@endcode.
     * @param maxBatchSize   The maximum number of log entries to accumulate
     *                       before a flush is triggered.
     *                       Defaults to @code 64@endcode.
     * @param flushIntervalMs The flush interval in milliseconds for the
     *                        background logger thread.
     *                        Defaults to @code 250 ms@endcode.
     */
    explicit Settings(
        Level minimumLevel = Level::kInfo,
        size_t maxBatchSize = 64,
        int flushIntervalMs = 250
    )   : _minimumLevel(minimumLevel)
        , _maxBatchSize(maxBatchSize)
        , _flushIntervalMs(flushIntervalMs)
    {}

    /// @return The minimum log level currently configured.
    [[nodiscard]] Level getMinimumLevel() const { return _minimumLevel; }

    /// @return The configured maximum batch size for log processing.
    [[nodiscard]] size_t getMaxBatchSize() const { return _maxBatchSize; }

    /// @return The flush interval in milliseconds.
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
