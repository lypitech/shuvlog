#ifndef SHUVLOG_LOGGER_H
#define SHUVLOG_LOGGER_H

#include <iostream>
#include <fstream>
#include <source_location>
#include <string>
#include <thread>

#include "BuildInfo.h"
#include "FileSink.h"
#include "Exceptions/LoggerException.h"
#include "Level.h"
#include "Log.h"
#include "Settings.h"
#include "Sink.h"
#include "ThreadSafeQueue.h"
#include "Exceptions/DuplicateSink.h"
#include "Sinks/ConsoleSink.h"

#define CUR_SOURCE      std::source_location::current()
#define LOG_DEBUG(...)  Logger::getInstance().log(logger::Level::kDebug,    CUR_SOURCE, __VA_ARGS__)
#define LOG_INFO(...)   Logger::getInstance().log(logger::Level::kInfo,     CUR_SOURCE, __VA_ARGS__)
#define LOG_WARN(...)   Logger::getInstance().log(logger::Level::kWarning,  CUR_SOURCE, __VA_ARGS__)
#define LOG_ERR(...)    Logger::getInstance().log(logger::Level::kError,    CUR_SOURCE, __VA_ARGS__)
#define LOG_CRIT(...)   Logger::getInstance().log(logger::Level::kCritical, CUR_SOURCE, __VA_ARGS__)
#define LOG_FATAL(...)  Logger::getInstance().log(logger::Level::kFatal,    CUR_SOURCE, __VA_ARGS__)

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

        try {
            constexpr bool isConsoleSink = std::is_same_v<T, logger::ConsoleSink>;

            if (isConsoleSink && _hasConsoleSink) {
                throw logger::exception::DuplicateSink("CONSOLE");
            }

            auto sink = std::make_shared<T>(std::forward<Args>(args)...);
            std::lock_guard lock(_sinkMutex);

            if constexpr (std::is_base_of_v<logger::FileSink, T>) {
                auto newFilepath = sink->getAbsoluteFilepath();

                for (const auto& s : _sinks) {
                    if (const auto fileSink = std::dynamic_pointer_cast<logger::FileSink>(s)) {
                        const std::string sinkFilepath = fileSink->getAbsoluteFilepath();
                        if (sinkFilepath != newFilepath) {
                            continue;
                        }

                        throw logger::exception::DuplicateSink(sinkFilepath);
                    }
                }
            }

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

    template<typename... Args>
    void log(
        logger::Level level,
        const std::source_location& loc,
        std::format_string<Args...> format,
        Args&&... args
    )
    {
        std::string message = std::format(format, std::forward<Args>(args)...);
        log(level, loc, std::string_view{message});
    }

    void log(
        logger::Level level,
        const std::source_location& loc,
        std::string_view message
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
