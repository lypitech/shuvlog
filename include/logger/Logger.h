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

// 16 args to be good for a good time. To refactor if ppl need more args!
#define LOG_GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, NAME, ...) NAME

#define LOG_DEBUG(...) LOG_GET_MACRO(__VA_ARGS__, \
    LOG_DEBUG_N, LOG_DEBUG_N, LOG_DEBUG_N, LOG_DEBUG_N, \
    LOG_DEBUG_N, LOG_DEBUG_N, LOG_DEBUG_N, LOG_DEBUG_N, \
    LOG_DEBUG_N, LOG_DEBUG_N, LOG_DEBUG_N, LOG_DEBUG_N, \
    LOG_DEBUG_N, LOG_DEBUG_N, LOG_DEBUG_N, LOG_DEBUG_1)(__VA_ARGS__)

#define LOG_INFO(...) LOG_GET_MACRO(__VA_ARGS__, \
    LOG_INFO_N, LOG_INFO_N, LOG_INFO_N, LOG_INFO_N, \
    LOG_INFO_N, LOG_INFO_N, LOG_INFO_N, LOG_INFO_N, \
    LOG_INFO_N, LOG_INFO_N, LOG_INFO_N, LOG_INFO_N, \
    LOG_INFO_N, LOG_INFO_N, LOG_INFO_N, LOG_INFO_1)(__VA_ARGS__)

#define LOG_ERR(...) LOG_GET_MACRO(__VA_ARGS__, \
    LOG_ERR_N, LOG_ERR_N, LOG_ERR_N, LOG_ERR_N, \
    LOG_ERR_N, LOG_ERR_N, LOG_ERR_N, LOG_ERR_N, \
    LOG_ERR_N, LOG_ERR_N, LOG_ERR_N, LOG_ERR_N, \
    LOG_ERR_N, LOG_ERR_N, LOG_ERR_N, LOG_ERR_1)(__VA_ARGS__)

#define LOG_WARN(...) LOG_GET_MACRO(__VA_ARGS__, \
    LOG_WARN_N, LOG_WARN_N, LOG_WARN_N, LOG_WARN_N, \
    LOG_WARN_N, LOG_WARN_N, LOG_WARN_N, LOG_WARN_N, \
    LOG_WARN_N, LOG_WARN_N, LOG_WARN_N, LOG_WARN_N, \
    LOG_WARN_N, LOG_WARN_N, LOG_WARN_N, LOG_WARN_1)(__VA_ARGS__)

#define LOG_CRIT(...) LOG_GET_MACRO(__VA_ARGS__, \
    LOG_CRIT_N, LOG_CRIT_N, LOG_CRIT_N, LOG_CRIT_N, \
    LOG_CRIT_N, LOG_CRIT_N, LOG_CRIT_N, LOG_CRIT_N, \
    LOG_CRIT_N, LOG_CRIT_N, LOG_CRIT_N, LOG_CRIT_N, \
    LOG_CRIT_N, LOG_CRIT_N, LOG_CRIT_N, LOG_CRIT_1)(__VA_ARGS__)

#define LOG_FATAL(...) LOG_GET_MACRO(__VA_ARGS__, \
    LOG_FATAL_N, LOG_FATAL_N, LOG_FATAL_N, LOG_FATAL_N, \
    LOG_FATAL_N, LOG_FATAL_N, LOG_FATAL_N, LOG_FATAL_N, \
    LOG_FATAL_N, LOG_FATAL_N, LOG_FATAL_N, LOG_FATAL_N, \
    LOG_FATAL_N, LOG_FATAL_N, LOG_FATAL_N, LOG_FATAL_1)(__VA_ARGS__)

// single argument versions (no formatting)
#define LOG_DEBUG_1(msg) Logger::getInstance().log(logger::Level::kDebug, std::string_view{msg})
#define LOG_INFO_1(msg)  Logger::getInstance().log(logger::Level::kInfo, std::string_view{msg})
#define LOG_ERR_1(msg)   Logger::getInstance().log(logger::Level::kError, std::string_view{msg})
#define LOG_WARN_1(msg)  Logger::getInstance().log(logger::Level::kWarning, std::string_view{msg})
#define LOG_CRIT_1(msg)  Logger::getInstance().log(logger::Level::kCritical, std::string_view{msg})
#define LOG_FATAL_1(msg) Logger::getInstance().log(logger::Level::kFatal, std::string_view{msg})

// multiple argument versions (with formatting)
#define LOG_DEBUG_N(fmt, ...) Logger::getInstance().log(logger::Level::kDebug, std::source_location::current(), fmt, __VA_ARGS__)
#define LOG_INFO_N(fmt, ...)  Logger::getInstance().log(logger::Level::kInfo, std::source_location::current(), fmt, __VA_ARGS__)
#define LOG_ERR_N(fmt, ...)   Logger::getInstance().log(logger::Level::kError, std::source_location::current(), fmt, __VA_ARGS__)
#define LOG_WARN_N(fmt, ...)  Logger::getInstance().log(logger::Level::kWarning, std::source_location::current(), fmt, __VA_ARGS__)
#define LOG_CRIT_N(fmt, ...)  Logger::getInstance().log(logger::Level::kCritical, std::source_location::current(), fmt, __VA_ARGS__)
#define LOG_FATAL_N(fmt, ...) Logger::getInstance().log(logger::Level::kFatal, std::source_location::current(), fmt, __VA_ARGS__)

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
        log(level, std::string_view{message}, loc);
    }

    void log(
        logger::Level level,
        std::string_view message,
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
