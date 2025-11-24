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

#define CUR_SOURCE          std::source_location::current()
#define LOG_DEBUG(...)      Logger::getInstance().log(logger::Level::kDebug,    CUR_SOURCE, __VA_ARGS__)
#define LOG_TRACE_L3(...)   Logger::getInstance().log(logger::Level::kTraceL3,  CUR_SOURCE, __VA_ARGS__)
#define LOG_TRACE_L2(...)   Logger::getInstance().log(logger::Level::kTraceL2,  CUR_SOURCE, __VA_ARGS__)
#define LOG_TRACE_L1(...)   Logger::getInstance().log(logger::Level::kTraceL1,  CUR_SOURCE, __VA_ARGS__)
#define LOG_INFO(...)       Logger::getInstance().log(logger::Level::kInfo,     CUR_SOURCE, __VA_ARGS__)
#define LOG_WARN(...)       Logger::getInstance().log(logger::Level::kWarning,  CUR_SOURCE, __VA_ARGS__)
#define LOG_ERR(...)        Logger::getInstance().log(logger::Level::kError,    CUR_SOURCE, __VA_ARGS__)
#define LOG_CRIT(...)       Logger::getInstance().log(logger::Level::kCritical, CUR_SOURCE, __VA_ARGS__)
#define LOG_FATAL(...)      Logger::getInstance().log(logger::Level::kFatal,    CUR_SOURCE, __VA_ARGS__)

/**
 * @class   Logger
 * @brief   Asynchronous, thread-safe, singleton logging engine.
 *
 * This class implements the central logging backend of the library.
 * It is responsible for:
 *   - Managing global logging settings (@code logger::Settings@endcode)
 *   - Creating and managing log sinks (e.g., console, Log file, JSON file...)
 *   - Formatting and dispatching log messages
 *   - Batching and asynchronously processing log events in a worker thread
 *   - Emitting startup headers and ensuring proper shutdown/flushing
 *
 * Each call to @code log()@endcode creates a Log entry and, consequently, records:
 *   - The formatted text message
 *   - The severity level
 *   - A @code std::source_location@endcode describing file, function, and line
 *   - Thread ID and user-defined thread label
 *   - A timestamp
 *
 * The Logger prevents:
 *   - Registering more than one console sink.
 *   - Registering multiple file sinks that have the same output file.
 *
 * Any violation will raise an instance of
 * @code logger::exception::LoggerException@endcode.
 * If initialization has completed, these errors are logged through the logger.
 * Otherwise, they are printed to the standard error output
 * (@code std::cerr@endcode).
 *
 * @section macros  Convenience Macros
 * The following macros automatically provide the level and source location:
 *   - @code LOG_DEBUG(...)@endcode
 *   - @code LOG_INFO(...)@endcode
 *   - @code LOG_WARN(...)@endcode
 *   - @code LOG_ERR(...)@endcode
 *   - @code LOG_CRIT(...)@endcode
 *   - @code LOG_FATAL(...)@endcode
 */
class Logger final
{
public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    static Logger& getInstance();

    /**
     * @brief   Initializes the Logger singleton and starts the logging backend.
     *
     * This function configures the Logger singleton and starts the
     * worker thread responsible for processing queued log events.
     *
     * Initialization also:
     *   - sets the current thread's label to "MainThread".
     *   - ensures that the log directory exists (and creates one if necessary).
     *   - calls @code writeHeader()@endcode on each registered sink.
     *
     * The function is guaranteed to run exactly once (@code std::once_flag@endcode).
     *
     * @warning This function MUST be called before any call to @code log@endcode.
     *          Calling it more than once has no effect.
     *
     * @param   projectName Name of the project that uses the Logger
     * @param   argc        Number of command-line arguments
     *                      (given in @code main()@endcode)
     * @param   argv        Array of command-line arguments
     *                      (given in @code main()@endcode)
     * @param   buildInfo   Build metadata
     * @param   settings    Global Logger settings
     */
    static void initialize(
        const std::string& projectName,
        int argc,
        const char* argv[],
        const logger::BuildInfo& buildInfo,
        const logger::Settings& settings = logger::Settings()
    );

    /**
     * @brief   Attaches a new Sink to the Logger.
     *
     * This function ensures that the Sink is not using another
     * already-registered Sink's output (prevents duplicates).
     *
     * @tparam  T       Class of the Sink to attach. T must inherit from
     *                  @code logger::Sink@endcode.
     * @param   args    Arguments that the Sink class takes as parameters
     */
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

            if (_isInitialized) {
                sink->writeHeader(
                    _projectName,
                    _argc,
                    _argv,
                    _buildInfo,
                    _settings
                );
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

    /**
     * @brief   Creates a log with a formatted message, and puts it in the
     *          queue.
     *
     * Acts the exact same way as the other @code log()@endcode function,
     * but with a format feature, that uses @code std::format@endcode.
     *
     * @param   level   Severity of the log message
     * @param   loc     Source information (file, line, function)
     * @param   format  Format
     * @param   args    Format arguments
     */
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

    /**
     * @brief   Creates a log message and puts it in the queue.
     *
     * @see     @class Log
     *
     * @param   level   Severity of the log message
     * @param   loc     Source information (file, line, function)
     * @param   message Log message
     */
    void log(
        logger::Level level,
        const std::source_location& loc,
        std::string_view message
    );

    /**
     * @brief   Shuts down the Logger.
     *
     * This function stops the worker thread, flushes all remaining queued log
     * events, and calls @code flush()@endcode on all sinks. After shutdown,
     * no other log messages will be processed.
     */
    void shutdown();

    // === helpers ===
    /**
     * @brief   Converts a level to a string.
     *
     * @param   level   Logging severity level
     * @return  Converted data (e.g., "INFO", "ERROR")
     */
    static std::string levelToString(logger::Level level);

    /**
     * @brief   Generates a timestamped filename for log output.
     *
     * The resulting filename includes the project name, current date/time,
     * and the file extension passed as parameter.
     *
     * Example: R-Type_Server_2024-01-16_14-22-53.log
     *
     * @param   projectName Name of the project using the logger
     * @param   extension   File extension, without dot
     *                      (e.g., "log", "json" and not ".log", ".json")
     * @return  Generated filename
     */
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
    int _argc = 0;
    const char** _argv = nullptr;

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
