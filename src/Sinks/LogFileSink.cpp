#include <iostream>

#include "logger/Exceptions/BadFileExtension.h"
#include "logger/Exceptions/LoggerException.h"
#include "logger/Logger.h"
#include "logger/OsInfo.h"
#include "logger/Sinks/LogFileSink.h"
#include "logger/Timestamp.h"

namespace logger
{

LogFileSink::LogFileSink(const std::string& filepath)
    : _file(filepath, std::ios::in | std::ios::out | std::ios::trunc)
{
    if (!_file.is_open()) {
        throw exception::LoggerException(std::format("{}: Could not open file.", filepath));
    }
    if (!filepath.ends_with(".log")) {
        throw exception::BadFileExtension("Log");
    }
}

static std::string formatLog(
    const Log& log,
    const Settings& settings
)
{
    std::string ts;
    if (settings.getShowTimestamp()) {
        ts = "[" + formatTimestamp(log.getTimestamp()) + "] ";
    }

    std::string src;
    if (settings.getShowSource()) {
        src = std::format(
            " ({}:{})",
            log.getLocation().file_name(), log.getLocation().line()
        );
    }

    return std::format(
        "{}<{}> {}{}\n",
        ts, Logger::levelToString(log.getLevel()),
        log.getMessage(), src
    );
}

void LogFileSink::write(
    const Log& log,
    const Settings& settings
)
{
    _file << formatLog(log, settings);
}

void LogFileSink::writeHeader(
    const std::string& projectName,
    const int argc,
    const char* argv[],
    const Settings& settings
)
{
    std::ostringstream header;

    header << "=== LOG START: " << formatTimestamp(system_clock::now()) << " ===\n";
    header << "Project: " << projectName << "\n";
    header << "Command: ";
    for (int i = 0; i < argc; i++) {
        header << argv[i] << (i + 1 == argc ? "" : " ");
    }
    header << "\n";
    header << "OS: " << osname() << " " << kernelver() << "\n";
    header << "Minimum log level: " << Logger::levelToString(settings.getMinimumLevel()) << "\n\n";

    _file << header.str();
}

void LogFileSink::flush()
{
    _file.flush();
}

void LogFileSink::close()
{
    if (_file.is_open()) {
        _file.close();
    }
}

}
