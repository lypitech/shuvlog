#include "logger/Logger.h"
#include "logger/Sinks/ConsoleSink.h"
#include "logger/Sinks/JsonFileSink.h"
#include "logger/Sinks/LogFileSink.h"
#include "logger/Sinks/NdJsonFileSink.h"

int main(const int argc, const char* argv[])
{
    logger::sink::Settings sinkSettings{
        .showOnlyTime = true,
        .showThreadId = false,
        .showSource = false,
        .showColumnNumber = false
    };

    Logger::getInstance().addSink<logger::ConsoleSink>(sinkSettings);
    // Logger::getInstance().addSink<logger::LogFileSink>(Logger::generateLogFileName("Tester3000", "log"));
    Logger::getInstance().addSink<logger::LogFileSink>("logs/latest.log", sinkSettings);
    // Logger::getInstance().addSink<logger::JsonFileSink>("logs/latest.json");
    // Logger::getInstance().addSink<logger::JsonFileSink>("logs/badjson.txt");
    // Logger::getInstance().addSink<logger::NdJsonFileSink>("logs/test.ndjson");

    Logger::initialize(
        "Tester3000",
        argc, argv,
        logger::BuildInfo::fromCMake(),
        logger::Settings(
            logger::Level::kDebug//,
            // 1,
            // 0
        )
    );

    LOG_DEBUG("Debug log.");
    LOG_INFO("Info log.");

    Logger::getInstance().addSink<logger::JsonFileSink>("logs/badjson2.txt");

    LOG_WARN("Warning log.");
    LOG_ERR("Error log.");
    LOG_CRIT("Critical log.");
    LOG_FATAL("Fatal log.");
    // raise(11);
    return 0;
}
