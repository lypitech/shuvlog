#include "logger/Logger.h"
#include "logger/Sinks/ConsoleSink.h"
#include "logger/Sinks/LogFileSink.h"
#include "logger/Sinks/JsonFileSink.h"
#include "logger/Sinks/NdJsonFileSink.h"

int main(const int argc, const char* argv[])
{
    using namespace logger;
    // logger::sink::Settings sinkSettings{
    //     .showTimestamp = false,
    //     .showThreadInfo = false,
    //     .showSource = false,
    // };

    // sink::Settings shortTimeSinkSettings{
    //     .showOnlyTime = true,
    //     .showMilliseconds = false
    // };

    // Logger::getInstance().addSink<ConsoleSink>(
    //     sink::FilterMode::kAll,
    //     Level::kTraceR1 | Level::kTraceR2 | Level::kTraceR3,
    //     true,
    //     shortTimeSinkSettings
    // );
    //
    // Logger::getInstance().addSink<ConsoleSink>(
    //     sink::FilterMode::kExplicit,
    //     Level::kTraceR1 | Level::kTraceR2 | Level::kError | Level::kFatal,
    //     true,
    //     shortTimeSinkSettings
    // );

    Logger::getInstance().addSink<ConsoleSink>(
        sink::FilterMode::kAll,
        Level::kDebug | Level::kTraceR1
    );
    // Logger::getInstance().addSink<logger::LogFileSink>(Logger::generateLogFileName("Tester3000", "log"));
    // Logger::getInstance().addSink<LogFileSink>(
    //     "logs/latest.log",
    //     sink::FilterMode::kMinimumLevel,
    //     static_cast<uint16_t>(Level::kTraceR1)
    // );
    // Logger::getInstance().addSink<JsonFileSink>(
    //     "logs/latest.json",
    //     sink::FilterMode::kMinimumLevel,
    //     static_cast<uint16_t>(Level::kTraceR1)
    // );
    // Logger::getInstance().addSink<JsonFileSink>("logs/latest.json");
    // Logger::getInstance().addSink<JsonFileSink>("logs/badjson.txt");
    // Logger::getInstance().addSink<NdJsonFileSink>("logs/test.ndjson");

    Logger::initialize(
        "Tester3000",
        argc, argv,
        BuildInfo::fromCMake()
        // logger::Settings(
        //     // 1,
        //     // 0
        // )
    );

    LOG_DEBUG("Debug log.");
    LOG_TRACE_R3("Trace 3 test");
    LOG_TRACE_R2("Trace 2 test");
    LOG_TRACE_R1("Trace 1 test");
    LOG_INFO("Info log.");
    LOG_WARN("Warning log.");
    LOG_ERR("Error log.");
    LOG_CRIT("Critical log.");
    LOG_FATAL("Fatal log.");

    // for (size_t k = 0; k < 100000; ++k) {
    //     LOG_FATAL("test {}", k);
    // }

    // raise(11);
    return 0;
}
