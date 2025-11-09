#ifndef SHOVOLOGGER_LOGGER_H
#define SHOVOLOGGER_LOGGER_H

#include <fstream>
#include <queue>
#include <source_location>
#include <string>
#include <thread>

#include "ThreadSafeQueue.h"

#define LOG_DEBUG(s)    Logger::getInstance().log(s, logger::Level::kDebug)
#define LOG_INFO(s)     Logger::getInstance().log(s, logger::Level::kInfo)
#define LOG_ERR(s)      Logger::getInstance().log(s, logger::Level::kError)
#define LOG_WARN(s)     Logger::getInstance().log(s, logger::Level::kWarning)
#define LOG_CRIT(s)     Logger::getInstance().log(s, logger::Level::kCritical)
#define LOG_FATAL(s)    Logger::getInstance().log(s, logger::Level::kFatal)

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

class Log final
{
public:
    Log(
        std::string message,
        logger::Level level,
        const std::source_location& loc
    );

    [[nodiscard]] std::string getMessage() const { return _message; }
    [[nodiscard]] logger::Level getLevel() const { return _level; }
    [[nodiscard]] std::source_location getLocation() const { return _location; }
    [[nodiscard]] std::chrono::time_point<std::chrono::system_clock> getTimestamp() const { return _timestamp; }

private:
    std::string _message;
    logger::Level _level;
    std::source_location _location;
    std::chrono::time_point<std::chrono::system_clock> _timestamp;
};

class Logger final
{
public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    static Logger& getInstance();

    static void initialize(
        const std::string& projectName,
        int argc,
        const char* argv[],
        logger::Settings settings = logger::Settings()
    );

    void log(
        std::string_view message,
        logger::Level level = logger::Level::kInfo,
        const std::source_location& loc = std::source_location::current()
    );

    void shutdown();

    [[nodiscard]] bool isInitialized() const { return _isInitialized; }
    [[nodiscard]] logger::Settings& getSettings() { return _settings; }

private:
    Logger() = default;
    ~Logger();

    // worker loop
    void workerLoop();

    void collectBatch(std::vector<Log>& batch);
    void collectRemainingLogs(std::vector<Log>& batch);
    void flushBatch(std::vector<Log>& batch);

    // formatting and sinks
    static std::string formatTimestamp(
        bool forFilename = false,
        std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now()
    ) ;
    static std::string levelToString(logger::Level level) ;
    std::string formatForConsole(const Log& log) const;
    std::string formatForFile(const Log& log) const;

    // helpers
    static std::string generateLogFileName(const std::string& projectName) ;
    void openLogFiles(const std::string& filepath);
    static std::string generateHeader(
        const Logger &instance,
        int argc,
        const char* argv[]
    ) ;

    // configuration
    logger::Settings _settings;
    std::string _projectName;

    // runtime
    std::thread _worker;
    ThreadSafeQueue<Log> _queue;

    std::ofstream _logFile;
    std::ofstream _latestLogFile;

    std::atomic<bool> _isRunning{false};
    std::atomic<bool> _isInitialized{false};

    static std::once_flag initFlag;
};

#endif //SHOVOLOGGER_LOGGER_H
