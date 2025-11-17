#include "logger/Exceptions/BadFileExtension.h"
#include "logger/Exceptions/CouldNotOpenFile.h"
#include "logger/Logger.h"
#include "logger/OsInfo.h"
#include "logger/Sinks/LogFileSink.h"
#include "logger/Timestamp.h"

#include <map>
// #include <__ranges/transform_view.h>

namespace logger
{

LogFileSink::LogFileSink(
    const std::string& filepath,
    sink::Settings settings
)
    : Sink(settings)
    , _file(filepath, std::ios::in | std::ios::out | std::ios::trunc)
{
    if (!_file.is_open()) {
        throw exception::CouldNotOpenFile(filepath);
    }
    if (!filepath.ends_with(".log")) {
        throw exception::BadFileExtension("Log");
    }
}

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
        Logger::levelToString(log.getLevel())
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

void LogFileSink::write(const Log& log)
{
    _file << formatLog(log, _settings);
}

void LogFileSink::writeHeader(
    const std::string& projectName,
    const int argc,
    const char* argv[],
    const BuildInfo& buildInfo,
    const Settings& settings
)
{
    std::ostringstream command;
    for (int k = 0; k < argc; k++) {
        command << argv[k];
    }
    std::vector<std::pair<std::string, std::string>> infos = {
        { "",                   ""                                                  },
        { "Project",            projectName                                         },
        { "Version",            buildInfo.getVersion()                              },
        { "Build type",         buildInfo.getType()                                 },
        { "Minimum level",      Logger::levelToString(settings.getMinimumLevel())   },
        { "",                   ""                                                  },
        { "Command",            command.str()                                       },
        { "Start time",         formatTimestamp(system_clock::now())                },
        { "",                   ""                                                  },
        { "OS",                 std::format("{} {}", osname(), kernelver())  },
        { "Compiler",           buildInfo.getCompiler()                             },
        { "Compilation flags",  buildInfo.getCompilerFlags()                        },
        { "Build system",       buildInfo.getBuildSystem()                          },
        { "",                   ""                                                  },
    };

    size_t maxKeyLen = 0;

    for (auto& [label, value] : infos) {
        if (label.size() > maxKeyLen) {
            maxKeyLen = label.size();
        }
    }

    std::ostringstream header;

    header << "/*************************************************\n";
    for (auto& [label, value] : infos) {
        if (label.empty()) {
            header << "|\n";
            continue;
        }
        header << std::format("|   {:{}}  :  {}\n", label, maxKeyLen, value);
    }
    header << "\\*************************************************\n\n";

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
