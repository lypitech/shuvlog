#include "logger/Exceptions/BadFileExtension.h"
#include "logger/Exceptions/CouldNotOpenFile.h"
#include "logger/Logger.h"
#include "logger/OsInfo.h"
#include "logger/Sinks/LogFileSink.h"
#include "logger/Timestamp.h"

#include <map>
#include <__ranges/transform_view.h>

namespace logger
{

LogFileSink::LogFileSink(const std::string& filepath)
    : _file(filepath, std::ios::in | std::ios::out | std::ios::trunc)
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
    const Settings& /*settings*/
)
{
    std::ostringstream oss;
    oss << log.getThreadId();

    return std::format(
        "{} [{} ({})] {:>8}: {} ({}:{})\n", // 8 is "CRITICAL"'s length (longest type)
        formatTimestamp(log.getTimestamp()),
        log.getThreadName(), oss.str(),
        Logger::levelToString(log.getLevel()),
        log.getMessage(),
        log.getLocation().file_name(), log.getLocation().line()
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

    size_t maxKeyLen = std::ranges::max(infos |
        std::ranges::views::transform([](auto const& p){
            return p.first.size();
        })
    );

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
