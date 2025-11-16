#include <filesystem>
#include <chrono>
#include <iostream>
#include <format>

#include "logger/Logger.h"
#include "logger/Thread.h"
#include "logger/Timestamp.h"

namespace fs = std::filesystem;

std::once_flag Logger::initFlag;

static const std::string LOG_DIR = "logs";

Logger& Logger::getInstance()
{
    static Logger instance;
    return instance;
}

void Logger::initialize(
    const std::string& projectName,
    const int argc,
    const char* argv[],
    const logger::BuildInfo& buildInfo,
    const logger::Settings& settings
)
{
    std::ios::sync_with_stdio(true);
    std::cerr.tie(&std::cout);

    std::call_once(initFlag, [&] {
        logger::setThreadLabel("MainThread");

        auto& instance = getInstance();

        instance._projectName = projectName;
        instance._settings = settings;

        if (!fs::exists(LOG_DIR)) {
            fs::create_directories(LOG_DIR);
        }

        for (const auto& sink : instance._sinks) {
            sink->writeHeader(instance._projectName, argc, argv, buildInfo, instance._settings);
        }

        instance._isRunning = true;
        instance._isInitialized = true;
        instance._worker = std::thread(&Logger::workerLoop, &instance);
    });
}

void Logger::log(
    const std::string_view message,
    logger::Level level,
    const std::source_location& loc
)
{
    if (!_isInitialized) {
        std::cerr << "CAUTION: Logger has been used uninitialized.\n"
                  << "Make sure you call the initialize() function before performing any log."
                  << std::endl;
        return;
    }

    if (_sinks.empty()) {
        std::cerr << "CAUTION: Trying to log with no sink."
                  << std::endl;
        return;
    }

    if (static_cast<uint8_t>(level) < static_cast<uint8_t>(_settings.getMinimumLevel())) {
        return;
    }

    _queue.push(Log{ std::string(message), level, loc });
}

void Logger::workerLoop()
{
    std::vector<Log> batch;
    batch.reserve(_settings.getMaxBatchSize());

    while (_isRunning) {
        // wait for new logs or timeout
        _queue.waitForData(milliseconds(_settings.getFlushIntervalMs()), _isRunning);
        // fetch logs into batch
        collectBatch(batch);
        // flush if there are logs
        if (!batch.empty()) {
            flushBatch(batch);
        }
    }
    collectRemainingLogs(batch);
    if (!batch.empty()) {
        flushBatch(batch);
    }
}

void Logger::collectBatch(std::vector<Log>& batch)
{
    while (batch.size() < _settings.getMaxBatchSize()) {
        std::optional<Log> log = _queue.pop();

        if (!log) {
            break;
        }
        batch.emplace_back(std::move(*log));
    }
}

void Logger::collectRemainingLogs(std::vector<Log>& batch)
{
    _queue.drainTo(batch);
}

void Logger::flushBatch(std::vector<Log>& batch)
{
    std::vector<std::shared_ptr<logger::Sink>> sinksCopy;

    // copying sinks references for outside writing safety
    {
        std::lock_guard lock(_sinkMutex);
        sinksCopy = _sinks;
    }

    for (Log& log : batch) {
        for (const auto& sink : _sinks) {
            sink->write(log);
        }
    }
    for (const auto& sink : sinksCopy) {
        sink->flush();
    }
    batch.clear();
}

std::string Logger::generateLogFileName(
    const std::string& projectName,
    const std::string& extension
)
{
    return std::format(
        "{}_{}.{}",
        projectName, formatTimestamp(system_clock::now(), true, true, true),
        extension
    );
}

std::string Logger::levelToString(const logger::Level level)
{
    switch (level) {
        using enum logger::Level;
        case kDebug:    return "DEBUG";
        case kInfo:     return "INFO";
        case kWarning:  return "WARNING";
        case kError:    return "ERROR";
        case kCritical: return "CRITICAL";
        case kFatal:    return "FATAL";
        default:        return "UNKNOWN";
    }
}

void Logger::shutdown()
{
    if (!_isInitialized) {
        return;
    }
    LOG_INFO(std::format("Shutting down Logger, will dump remaining logs ({}).", _queue.size()));
    _isRunning = false;
    _queue.notifyAll();
    if (_worker.joinable()) {
        _worker.join();
    }
    for (const auto& sink : _sinks) {
        sink->close();
    }
    _isInitialized = false;
}

Logger::~Logger()
{
    shutdown();
}
