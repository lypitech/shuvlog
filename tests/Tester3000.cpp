#include "logger/Logger.h"
#include "logger/Sinks/ConsoleSink.h"
#include "logger/Sinks/JsonFileSink.h"
#include "logger/Sinks/LogFileSink.h"

int main(const int argc, const char *argv[])
{
    Logger::getInstance().addSink(std::make_shared<logger::ConsoleSink>());
    Logger::getInstance().addSink(std::make_shared<logger::LogFileSink>(
        Logger::generateLogFileName("Tester3000")));
    Logger::getInstance().addSink(std::make_shared<logger::LogFileSink>("logs/latest.log"));
    Logger::getInstance().addSink(std::make_shared<logger::JsonFileSink>("logs/latest.json"));

    Logger::initialize(
        "Tester3000",
        argc, argv,
        logger::BuildInfo::fromCMake(),
        logger::Settings(
            logger::Level::kDebug,
            true,
            true,
            false,
            1,
            0
        )
    );

    LOG_DEBUG("Debug log.");
    LOG_INFO("Info log.");
    LOG_WARN("Warning log.");
    LOG_ERR("Error log.");
    LOG_CRIT("Critical log.");
    LOG_FATAL("Fatal log.");
    // raise(11);
    return 0;
}
