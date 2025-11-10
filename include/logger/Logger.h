#ifndef SHOVOLOGGER_LOGGER_H
#define SHOVOLOGGER_LOGGER_H

#include <fstream>
#include <source_location>
#include <string>
#include <thread>

#include "Level.h"
#include "Log.h"
#include "Settings.h"
#include "Sink.h"
#include "ThreadSafeQueue.h"

#define LOG_DEBUG(s)    Logger::getInstance().log(s, logger::Level::kDebug)
#define LOG_INFO(s)     Logger::getInstance().log(s, logger::Level::kInfo)
#define LOG_ERR(s)      Logger::getInstance().log(s, logger::Level::kError)
#define LOG_WARN(s)     Logger::getInstance().log(s, logger::Level::kWarning)
#define LOG_CRIT(s)     Logger::getInstance().log(s, logger::Level::kCritical)
#define LOG_FATAL(s)    Logger::getInstance().log(s, logger::Level::kFatal)

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

    void addSink(std::shared_ptr<logger::Sink> sink);

    void log(
        std::string_view message,
        logger::Level level = logger::Level::kInfo,
        const std::source_location& loc = std::source_location::current()
    );

    void shutdown();

    // helpers
    static std::string levelToString(logger::Level level);
    static std::string generateLogFileName(const std::string& projectName);

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

    // configuration
    logger::Settings _settings;
    std::string _projectName;

    // runtime
    std::thread _worker;
    ThreadSafeQueue<Log> _queue;

    std::vector<std::shared_ptr<logger::Sink>> _sinks;
    std::mutex _sinkMutex;

    std::atomic<bool> _isRunning{false};
    std::atomic<bool> _isInitialized{false};

    static std::once_flag initFlag;
};

#endif //SHOVOLOGGER_LOGGER_H
