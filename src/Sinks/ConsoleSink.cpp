#include "logger/Sinks/ConsoleSink.h"

#include <iostream>

#include "logger/Logger.h"
#include "logger/Timestamp.h"

namespace logger
{

ConsoleSink::ConsoleSink(Settings settings)
    : Sink(settings)
{}

static std::string formatLog(
    const Log& log,
    const Settings& /*settings*/
)
{
    return std::format(
        "{} [{}] {:>8}: {} ({}:{})\n", // 8 is "CRITICAL"'s length (longest type)
        formatTimestamp(log.getTimestamp()),
        log.getThreadName(),
        Logger::levelToString(log.getLevel()),
        log.getMessage(),
        log.getLocation().file_name(), log.getLocation().line()
    );
}

void ConsoleSink::write(const Log& log, const Settings& settings)
{
    std::ostream& out =
        static_cast<uint8_t>(log.getLevel()) >= static_cast<uint8_t>(Level::kError)
            ? std::cerr
            : std::cout;

    const std::string output = formatLog(log, settings);

    out << output;
}

void ConsoleSink::writeHeader(
    const std::string& /*projectName*/,
    int /*argc*/,
    const char* /*argv*/[],
    const BuildInfo& /*buildInfo*/,
    const Settings& /*settings*/
) {}

void ConsoleSink::flush()
{
    std::cout.flush();
    std::cerr.flush();
}

void ConsoleSink::close() {}

}
