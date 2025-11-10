#include "logger/Sinks/ConsoleSink.h"

#include <iostream>

#include "logger/Logger.h"
#include "logger/Timestamp.h"

namespace logger
{

static std::string formatLog(
    const Log& log,
    const Settings& settings
)
{
    const bool isOutputColored = settings.getColoredOutput();
    std::string prefix;

    if (settings.getShowTimestamp()) {
        prefix += "[" + formatTimestamp(log.getTimestamp()) + "] ";
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
        Logger::levelToString(log.getLevel())
    );

    std::string source;
    if (settings.getShowSource()) {
        source = std::format(
            " ({}:{})",
            log.getLocation().file_name(), log.getLocation().line()
        );
    }

    std::string output = std::format(
        "{}{}{}{}\n",
        prefix, log.getMessage(), source,
        isOutputColored ? "\033[0m" : ""
    );

    return output;
}

void ConsoleSink::write(const Log &log, const Settings &settings)
{
    std::ostream& out =
        static_cast<uint8_t>(log.getLevel()) >= static_cast<uint8_t>(Level::kError)
            ? std::cerr
            : std::cout;

    std::string output = formatLog(log, settings);

    out << output;
}

void ConsoleSink::writeHeader(
    const std::string &/*projectName*/,
    int /*argc*/,
    const char */*argv*/[],
    const Settings &/*settings*/
) {}

void ConsoleSink::flush()
{
    std::cout.flush();
    std::cerr.flush();
}

void ConsoleSink::close() {}

}
