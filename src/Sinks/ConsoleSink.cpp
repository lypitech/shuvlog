#include "logger/Sinks/ConsoleSink.h"

#include <iostream>

#include "logger/Logger.h"
#include "logger/Timestamp.h"

namespace logger
{

ConsoleSink::ConsoleSink(sink::Settings settings)
    : Sink(settings)
{}

ConsoleSink::ConsoleSink(
    sink::FilterMode filterMode,
    uint16_t levelMask,
    sink::Settings settings
)
    : Sink(filterMode, levelMask, settings)
{}

static std::string formatLog(
    const Log& log,
    const sink::Settings& settings
)
{
    std::string output;
    output.reserve(192); // avoids the first necessary dynamic allocations, to save some cpu cycles

    if (settings.showTimestamp) {
        std::format_to(std::back_inserter(output),
            "{} ",
            formatTimestamp(
                log.getTimestamp(),
                settings.showOnlyTime,
                settings.showMilliseconds
            )
        );
    }

    if (settings.showThreadInfo) {
        if (settings.showThreadId) {
            std::format_to(std::back_inserter(output),
                "[{} ({})] ",
                log.getThreadName(),
                std::hash<std::thread::id>{}(log.getThreadId())
            );
        } else {
            std::format_to(std::back_inserter(output),
                "[{}] ",
                log.getThreadName()
            );
        }
    }

    std::format_to(std::back_inserter(output),
        "{:>8}: ",
        level::to_string(log.getLevel())
    );

    output += log.getMessage();

    if (settings.showSource) {
        const auto& loc = log.getLocation();

        output += " (";
        output += loc.file_name();

        if (settings.showLineNumber) {
            std::format_to(std::back_inserter(output),
                ":{}",
                loc.line()
            );
        }
        if (settings.showColumnNumber) {
            std::format_to(std::back_inserter(output),
                ":{}",
                loc.column()
            );
        }
        output += ")";
    }

    output += "\n";

    return output;
}

void ConsoleSink::write(const Log& log)
{
    std::ostream& out =
        static_cast<uint8_t>(log.getLevel()) >= static_cast<uint8_t>(Level::kError)
            ? std::cerr
            : std::cout;

    const std::string output = formatLog(log, _settings);

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
