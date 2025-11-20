#include "logger/Logger.h"
#include "logger/Sinks/ConsoleSink.h"

int main(const int argc, const char* argv[]) {
    logger::sink::Settings shortTimeSinkSettings{
        .showOnlyTime = true,
        .showMilliseconds = false
    };

    Logger::getInstance().addSink<logger::ConsoleSink>();
    Logger::getInstance().addSink<logger::ConsoleSink>(shortTimeSinkSettings); // To differentiate this Sink from the other one.

    Logger::initialize("Bug demonstration", argc, argv, logger::BuildInfo::unknown());

    LOG_ERR("Duplicate log.");
    return 0;
}

// int main(const int argc, const char* argv[])
// {
//     // logger::sink::Settings sinkSettings{
//     //     .showTimestamp = false,
//     //     .showThreadInfo = false,
//     //     .showSource = false,
//     // };
//
//     logger::sink::Settings shortTimeSinkSettings{
//         .showOnlyTime = true,
//         .showMilliseconds = false
//     };
//
//     Logger::getInstance().addSink<logger::ConsoleSink>(shortTimeSinkSettings);
//     Logger::getInstance().addSink<logger::ConsoleSink>();
//     // Logger::getInstance().addSink<logger::LogFileSink>(Logger::generateLogFileName("Tester3000", "log"));
//     // Logger::getInstance().addSink<logger::LogFileSink>("logs/latest.log", sinkSettings);
//     // Logger::getInstance().addSink<logger::JsonFileSink>("logs/latest.json");
//     // Logger::getInstance().addSink<logger::JsonFileSink>("logs/badjson.txt");
//     // Logger::getInstance().addSink<logger::NdJsonFileSink>("logs/test.ndjson");
//
//     Logger::initialize(
//         "Tester3000",
//         argc, argv,
//         logger::BuildInfo::fromCMake()
//         // logger::Settings(
//         //     logger::Level::kDebug//,
//         //     // 1,
//         //     // 0
//         // )
//     );
//
//     LOG_DEBUG("Duplicate log.");
//
//     // LOG_DEBUG("Debug log.");
//     // LOG_INFO("Info log.");
//     //
//     // Logger::getInstance().addSink<logger::JsonFileSink>("logs/badjson2.txt");
//     //
//     // LOG_WARN("Warning log.");
//     // LOG_ERR("Error log.");
//     // LOG_CRIT("Critical log.");
//     // LOG_FATAL("Fatal log.");
//     // raise(11);
//     return 0;
// }
