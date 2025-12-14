#include <iostream>
#if defined(_WIN32)
#include <windows.h>
#endif

#include "logger/Sinks/ConsoleSink.h"
#include "logger/Logger.h"
#include "logger/Timestamp.h"
#include "logger/Thread.h"

namespace logger
{

ConsoleSink::ConsoleSink(
    bool useColors,
    sink::Settings settings
)
    : Sink(settings)
    , _useColors(useColors)
{}

ConsoleSink::ConsoleSink(
    sink::FilterMode filterMode,
    uint16_t levelMask,
    bool useColors,
    sink::Settings settings
)
    : Sink(filterMode, levelMask, settings)
    , _useColors(useColors)
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
                getPrettyThreadId(log.getThreadId())
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

    if (_useColors) {
        out << level::getColor(log.getLevel());
    }

    out << output;

    if (_useColors) {
        out << SHUVLOG_RST;
    }
}

void ConsoleSink::writeHeader(
    const std::string& /*projectName*/,
    int /*argc*/,
    const char* /*argv*/[],
    const BuildInfo& /*buildInfo*/,
    const Settings& /*settings*/
)
{
#if defined(_WIN32)
    // enabling ANSI escape codes on Windows 10+. Won't work on previous versions though, fixme:...
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

void ConsoleSink::flush()
{
    std::cout.flush();
    std::cerr.flush();
}

void ConsoleSink::close() {}

}
