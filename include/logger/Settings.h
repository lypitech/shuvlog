#ifndef SHOVOLOGGER_SETTINGS_H
#define SHOVOLOGGER_SETTINGS_H

#include "Level.h"

namespace logger
{

class Settings final
{
public:
    explicit Settings(
        Level minimumLevel = Level::kInfo,
        bool showTimestamp = true,
        bool showSource = true,
        bool coloredOutput = false,
        size_t maxBatchSize = 64,
        int flushIntervalMs = 250
    );

    [[nodiscard]] Level getMinimumLevel() const { return _minimumLevel; }
    [[nodiscard]] bool getShowTimestamp() const { return _showTimestamp; }
    [[nodiscard]] bool getShowSource() const { return _showSource; }
    [[nodiscard]] bool getColoredOutput() const { return _coloredOutput; }
    [[nodiscard]] size_t getMaxBatchSize() const { return _maxBatchSize; }
    [[nodiscard]] int getFlushIntervalMs() const { return _flushIntervalMs; }

    void setMinimumLevel(Level minimumLevel);
    void setShowTimestamp(bool showTimestamp);
    void setShowSource(bool showSource);
    void setColoredOutput(bool colored);
    void setMaxBatchSize(size_t maxBatchSize);
    void setFlushIntervalMs(int flushIntervalMs);

private:
    Level _minimumLevel = Level::kInfo;
    bool _showTimestamp = true;
    bool _showSource = true;
    bool _coloredOutput = true;

    size_t _maxBatchSize = 64;
    int _flushIntervalMs = 250;
};

}

#endif //SHOVOLOGGER_SETTINGS_H
