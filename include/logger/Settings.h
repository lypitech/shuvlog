#ifndef SHUVLOG_SETTINGS_H
#define SHUVLOG_SETTINGS_H

#include <cstdint>

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
     * @param maxBatchSize   The maximum number of log entries to accumulate
     *                       before a flush is triggered.
     *                       Defaults to @code 64@endcode.
     * @param flushIntervalMs The flush interval in milliseconds for the
     *                        background logger thread.
     *                        Defaults to @code 250 ms@endcode.
     */
    explicit Settings(
        size_t maxBatchSize = 64,
        int flushIntervalMs = 250
    )   : _maxBatchSize(maxBatchSize)
        , _flushIntervalMs(flushIntervalMs)
    {}

    /// @return The configured maximum batch size for log processing.
    [[nodiscard]] size_t getMaxBatchSize() const { return _maxBatchSize; }

    /// @return The flush interval in milliseconds.
    [[nodiscard]] int getFlushIntervalMs() const { return _flushIntervalMs; }

    void setMaxBatchSize(size_t maxBatchSize) { _maxBatchSize = maxBatchSize; }
    void setFlushIntervalMs(int flushIntervalMs) { _flushIntervalMs = flushIntervalMs; }

private:
    size_t _maxBatchSize;
    int _flushIntervalMs;
};

}

#endif //SHUVLOG_SETTINGS_H
