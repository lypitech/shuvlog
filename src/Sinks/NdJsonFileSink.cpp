#include "logger/Exceptions/BadFileExtension.h"
#include "logger/Exceptions/CouldNotOpenFile.h"
#include "logger/Logger.h"
#include "logger/OsInfo.h"
#include "logger/Sinks/NdJsonFileSink.h"
#include "logger/Timestamp.h"

namespace logger
{

static const char* EXTENSION_NAME = "NDJSON";
static const char* RECOMMENDED_EXTENSION = ".ndjson";

NdJsonFileSink::NdJsonFileSink(
    const std::string &filepath,
    sink::Settings settings
)
    : FileSink(
        filepath,
        EXTENSION_NAME,
        RECOMMENDED_EXTENSION,
        settings
    )
{}

NdJsonFileSink::NdJsonFileSink(
    const std::string &filepath,
    sink::FilterMode filterMode,
    uint16_t levelSpec,
    sink::Settings settings
)
    : FileSink(
        filepath,
        EXTENSION_NAME,
        RECOMMENDED_EXTENSION,
        filterMode,
        levelSpec,
        settings
    )
{}

static std::string formatLog(const Log& log)
{
    std::ostringstream oss;

    oss << "{";
    oss << R"("timestamp":")" << formatTimestamp(log.getTimestamp()) << "\",";
    oss << R"("type":")" << Logger::levelToString(log.getLevel()) << "\",";
    oss << R"("thread":{"name":")" << log.getThreadName() << R"(","id":")" << log.getThreadId() << "\"},";
    oss << R"("source":")" << log.getLocation().file_name() << "\",";
    oss << R"("functionName":")" << log.getLocation().function_name() << "\",";
    oss << R"("line":)" << log.getLocation().line() << ",";
    oss << R"("column":)" << log.getLocation().column() << ",";
    oss << R"("message":")" << log.getMessage() << "\"";
    oss << "}";
    return oss.str();
}

void NdJsonFileSink::write(const Log& log)
{
    _file << formatLog(log) << "\n";
}

void NdJsonFileSink::writeHeader(
    const std::string& projectName,
    const int argc,
    const char* argv[],
    const BuildInfo& buildInfo,
    const Settings& /*settings*/
)
{
    std::ostringstream body;

    body << "{";
    body << R"("projectName":")" << projectName << "\",";
    body << R"("version":")" << buildInfo.getVersion() << "\",";
    body << R"("buildType":")" << buildInfo.getType() << "\",";

    body << R"("command":")";
    for (int i = 0; i < argc; i++) {
        body << argv[i] << (i + 1 == argc ? "" : " ");
    }
    body << "\",";
    body << R"("startTime":")" << formatTimestamp(system_clock::now()) << "\",";

    body << R"("osName":")" << osname() << "\",";
    body << R"("kernelVersion":")" << kernelver() << "\",";
    body << R"("compiler":")" << buildInfo.getCompiler() << "\",";
    body << R"("compilationFlags":")" << buildInfo.getCompilerFlags() << "\",";
    body << R"("buildSystem":")" << buildInfo.getBuildSystem() << "\"";

    body << "}\n";

    _file << body.str();
}

void NdJsonFileSink::flush()
{
    _file.flush();
}

void NdJsonFileSink::close()
{
    if (_file.is_open()) {
        _file.close();
    }
}

}
