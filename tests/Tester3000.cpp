#include "logger/Logger.h"

int main(int argc, const char* argv[])
{
    Logger::initialize(
        "Tester3000",
        argc, argv,
        logger::Settings(
            logger::Level::kDebug
        )
    );

    LOG_DEBUG("Debug log.");
    LOG_INFO("Info log.");
    LOG_WARN("Warning log.");
    LOG_ERR("Error log.");
    LOG_CRIT("Critical log.");
    LOG_FATAL("Fatal log.");
    return 0;
}
