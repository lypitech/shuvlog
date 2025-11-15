#include "logger/Exceptions/BadFileExtension.h"
#include "logger/Exceptions/LoggerException.h"
#include "logger/Logger.h"
#include "logger/OsInfo.h"
#include "logger/Sinks/JsonFileSink.h"
#include "logger/Timestamp.h"

namespace logger
{

JsonFileSink::JsonFileSink(const std::string &filepath)
    : _file(filepath, std::ios::in | std::ios::out | std::ios::trunc)
{
    if (!_file.is_open()) {
        throw exception::LoggerException(std::format("{}: Could not open file.", filepath));
    }
    if (!filepath.ends_with(".json")) {
        throw exception::BadFileExtension("JSON");
    }
}

static std::string formatLog(const Log& log)
{
    std::ostringstream oss;

    oss << "{";
    oss << R"("timestamp":")" << formatTimestamp(log.getTimestamp()) << "\",";
    oss << R"("type":")" << Logger::levelToString(log.getLevel()) << "\",";
    oss << R"("source":")" << log.getLocation().file_name() << "\",";
    oss << R"("functionName":")" << log.getLocation().function_name() << "\",";
    oss << R"("line":)" << log.getLocation().line() << ",";
    oss << R"("column":)" << log.getLocation().column() << ",";
    oss << R"("message":")" << log.getMessage() << "\"";
    oss << "}";
    return oss.str();
}

void JsonFileSink::write(
    const Log& log,
    const Settings& /*settings*/ // in a JSON (machine-readable code), we need
                                 // all the information the Logger can possibly
                                 // provide.
)
{
    char c;

    _file.seekg(-3, std::ios::end);
    _file.get(c);
    _file.seekp(-2, std::ios::end);
    if (c != '[') {
        _file << ",";
    }
    _file << formatLog(log) << "]}";
}

void JsonFileSink::writeHeader(
    const std::string &projectName,
    const int argc,
    const char* argv[],
    const BuildInfo& /*buildInfo*/,
    const Settings& settings
)
{
    std::ostringstream body;

    body << "{";
    body << R"("projectName":")" << projectName << "\",";
    body << R"("createdAt":")" << formatTimestamp(system_clock::now()) << "\",";

    body << R"("commandUsed":")";
    for (int i = 0; i < argc; i++) {
        body << argv[i] << (i + 1 == argc ? "" : " ");
    }
    body << "\",";

    body << R"("osName":")" << osname() << "\",";
    body << R"("kernelVersion":")" << kernelver() << "\",";
    body << R"("minimumLevel":")" << Logger::levelToString(settings.getMinimumLevel()) << "\",";

    body << R"("logs":[])";

    body << "}";

    _file << body.str();
}

void JsonFileSink::flush()
{
    _file.flush();
}

void JsonFileSink::close()
{
    if (_file.is_open()) {
        _file.close();
    }
}

}
