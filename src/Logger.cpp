#include <filesystem>
#include <chrono>
#include <iostream>
#include <sstream>
#include <format>

#include "logger/Logger.h"
#include "logger/OsInfo.h"
#include "logger/Timestamp.h"
#include "logger/Exceptions/LoggerException.h"

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
    const logger::Settings settings
)
{
    std::ios::sync_with_stdio(true);
    std::cerr.tie(&std::cout);

    std::call_once(initFlag, [&] {
        auto& instance = getInstance();

        instance._projectName = projectName;
        instance._settings = settings;

        if (!fs::exists(LOG_DIR)) {
            fs::create_directories(LOG_DIR);
        }

        const std::string filename = generateLogFileName(projectName);
        try {
            instance.openLogFiles(filename);
        } catch (const std::exception&) {
            throw; // Letting user decide what to do.
        }

        const std::string header = generateHeader(instance, argc, argv);

        if (instance._logFile.is_open()) {
            instance._logFile << header;
            instance._logFile.flush();
        }
        if (instance._latestLogFile.is_open()) {
            instance._latestLogFile << header;
            instance._latestLogFile.flush();
        }

        instance._isRunning = true;
        instance._isInitialized = true;
        instance._worker = std::thread(&Logger::workerLoop, &instance);
    });
}

std::string Logger::generateHeader(
    const Logger& instance,
    const int argc,
    const char* argv[]
)
{
    std::ostringstream header;

    header << "=== LOG START: " << formatTimestamp() << " ===\n";
    header << "Project: " << instance._projectName << "\n";
    header << "Command: ";
    for (int i = 0; i < argc; i++) {
        header << argv[i] << (i + 1 == argc ? "" : " ");
    }
    header << "\n";
    header << "OS: " << osname() << " " << kernelver() << "\n";
    header << "Minimum log level: " << levelToString(instance._settings.getMinimumLevel()) << "\n\n";

    return header.str();
}

void Logger::openLogFiles(const std::string& filepath) {
    _logFile.open(filepath, std::ios::app);
    if (!_logFile.is_open()) {
        throw logger::exception::LoggerException("Could not open log file: " + filepath);
    }

    _latestLogFile.open(std::format("{}/latest.log", LOG_DIR), std::ios::trunc);
    if (!_latestLogFile.is_open()) {
        throw logger::exception::LoggerException("Could not open latest.log");
    }
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
    for (Log& log : batch) {
        const std::string fileLine = formatForFile(log);
        const std::string consoleLine = formatForConsole(log);

        std::ostream& out =
            static_cast<uint8_t>(log.getLevel()) >= static_cast<uint8_t>(logger::Level::kError)
                ? std::cerr
                : std::cout;

        out << consoleLine;

        if (_logFile.is_open()) {
            _logFile << fileLine;
        }
        if (_latestLogFile.is_open()) {
            _latestLogFile << fileLine;
        }
    }

    if (_logFile.is_open()) {
        _logFile.flush();
    }
    if (_latestLogFile.is_open()) {
        _latestLogFile.flush();
    }
    batch.clear();
}

std::string Logger::generateLogFileName(const std::string& projectName)
{
    const std::string filename = std::format(
        "{}/{}_{}.log",
        LOG_DIR, projectName, formatTimestamp(true)
    );
    return filename;
}

std::string Logger::levelToString(const logger::Level level)
{
    using namespace logger;

    switch (level) {
        case Level::kDebug:     return "DEBUG";
        case Level::kInfo:      return "INFO";
        case Level::kWarning:   return "WARNING";
        case Level::kError:     return "ERROR";
        case Level::kCritical:  return "CRITICAL";
        case Level::kFatal:     return "FATAL";
        default:                return "UNKNOWN";
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
    _isInitialized = false;
}

Logger::~Logger()
{
    shutdown();
    if (_logFile.is_open()) {
        _logFile.close();
    }
    if (_latestLogFile.is_open()) {
        _latestLogFile.close();
    }
}

std::string Logger::formatTimestamp(
    const bool forFilename,
    const time_point<system_clock> timestamp
)
{
    const std::tm tm = fromTimePoint(timestamp);
    std::ostringstream oss;

    if (forFilename) {
        oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
    } else {
        const auto ms = duration_cast<milliseconds>(timestamp.time_since_epoch()) % 1000;

        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
            << '.' << std::setw(3) << std::setfill('0') << ms.count();
    }
    return oss.str();
}

std::string Logger::formatForConsole(const Log &log) const
{
    using namespace logger;

    const bool isOutputColored = _settings.getColoredOutput();
    std::string prefix;

    if (_settings.getShowTimestamp()) {
        prefix += "[" + formatTimestamp(false, log.getTimestamp()) + "] ";
    }
    if (isOutputColored) {
        switch (log.getLevel()) {
            case Level::kDebug:     prefix += "\033[34m";       break; // blue
            case Level::kWarning:   prefix += "\033[33m";       break; // yellow
            case Level::kError:
            case Level::kCritical:  prefix += "\033[31m";       break; // red
            case Level::kFatal:     prefix += "\033[41;97m";    break; // red BG white text
            default:                                            break;
        }
    }

    prefix += std::format(
        "<{}> ",
        levelToString(log.getLevel())
    );

    std::string sourcePart;
    if (_settings.getShowSource()) {
        sourcePart = std::format(
            " ({}:{})",
            log.getLocation().file_name(), log.getLocation().line()
        );
    }

    std::string output = std::format(
        "{}{}{}{}\n",
        prefix, log.getMessage(), sourcePart,
        isOutputColored ? "\033[0m" : ""
    );
    return output;
}

std::string Logger::formatForFile(const Log &log) const
{
    std::string ts;
    if (_settings.getShowTimestamp()) {
        ts = "[" + formatTimestamp(false, log.getTimestamp()) + "] ";
    }

    std::string src;
    if (_settings.getShowSource()) {
        src = std::format(
            " ({}:{})",
            log.getLocation().file_name(), log.getLocation().line()
        );
    }

    return std::format(
        "{}<{}> {}{}\n",
        ts, levelToString(log.getLevel()),
        log.getMessage(), src
    );
}
