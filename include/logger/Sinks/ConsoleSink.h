#ifndef SHUVLOG_CONSOLESINK_H
#define SHUVLOG_CONSOLESINK_H

#include "../Sink.h"

namespace logger
{

/**
 * @class   ConsoleSink
 *
 * This sink writes log messages directly to the console (@code stdout@endcode
 * or @code stderr@endcode, depending on severity or formatting rules).
 *
 * @warning Only one ConsoleSink may be registered at a time. Attempting to add
 *          multiple instances results in a @code DuplicateSink@endcode error.
 */
class ConsoleSink : public Sink
{
public:
    explicit ConsoleSink(
        bool useColors = true,
        sink::Settings settings = sink::Settings()
    );

    explicit ConsoleSink(
        sink::FilterMode filterMode,
        uint16_t levelMask,
        bool useColors = true,
        sink::Settings settings = sink::Settings()
    );

    void write(const Log& log) override;
    void writeHeader(
        const std::string& projectName,
        int argc,
        const char* argv[],
        const BuildInfo& buildInfo,
        const Settings& settings
    ) override;
    void flush() override;
    void close() override;

private:
    bool _useColors;
};

}

#endif //SHUVLOG_CONSOLESINK_H
