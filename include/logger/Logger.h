#ifndef SHUVLOG_LOGGER_H
#define SHUVLOG_LOGGER_H

#include <iostream>
#include <fstream>
#include <source_location>
#include <string>
#include <thread>

#include "BuildInfo.h"
#include "Exceptions/LoggerException.h"
#include "Level.h"
#include "Log.h"
#include "Settings.h"
#include "Sink.h"
#include "ThreadSafeQueue.h"
#include "Sinks/ConsoleSink.h"

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
        const logger::BuildInfo& buildInfo,
        const logger::Settings& settings = logger::Settings()
    );

    template<typename T, typename... Args>
    void addSink(Args... args)
    {
        static_assert(
            std::is_base_of_v<logger::Sink, T>,
            "T must inherit from logger::Sink."
        );

        constexpr bool isConsoleSink = std::is_same_v<T, logger::ConsoleSink>;

        // if user wants to add a second console sink
        if (isConsoleSink && _hasConsoleSink) {
            const std::string error("Only one console sink can be added.");

            if (_isInitialized && !_sinks.empty()) {
                LOG_WARN(error);
            } else {
                std::cerr << "WARNING: " << error << std::endl;
            }
            return;
        }

        try {
            auto sink = std::make_unique<T>(std::forward<Args>(args)...);
            std::lock_guard lock(_sinkMutex);

            _sinks.push_back(std::move(sink));

            if (isConsoleSink) {
                _hasConsoleSink = true;
            }
        } catch (const logger::exception::LoggerException& e) {
            const std::string error = std::format("Encountered an error while adding Sink: {}", e.what());

            if (_isInitialized && !_sinks.empty()) {
                LOG_WARN(error);
            } else {
                std::cerr << error << std::endl;
            }
        }
    }

    void log(
        std::string_view message,
        logger::Level level = logger::Level::kInfo,
        const std::source_location& loc = std::source_location::current()
    );

    void shutdown();

    // helpers
    static std::string levelToString(logger::Level level);
    static std::string generateLogFileName(const std::string& projectName, const std::string& extension);

    [[nodiscard]] bool isInitialized() const { return _isInitialized; }
    [[nodiscard]] logger::Settings& getSettings() { return _settings; }

private:
    Logger() = default;
    ~Logger();

    void workerLoop();

    void collectBatch(std::vector<Log>& batch);
    void collectRemainingLogs(std::vector<Log>& batch);
    void flushBatch(std::vector<Log>& batch);

    // configuration
    logger::Settings _settings;
    std::string _projectName;
    logger::BuildInfo _buildInfo = logger::BuildInfo::unknown();

    // runtime
    std::thread _worker;
    ThreadSafeQueue<Log> _queue;

    std::vector<std::shared_ptr<logger::Sink>> _sinks;
    std::mutex _sinkMutex;

    std::atomic<bool> _isRunning{false};
    std::atomic<bool> _isInitialized{false};
    bool _hasConsoleSink = false;

    static std::once_flag initFlag;
};

#endif //SHUVLOG_LOGGER_H
