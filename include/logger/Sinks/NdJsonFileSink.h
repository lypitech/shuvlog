#ifndef SHOVOLOGGER_NDJSONFILESINK_H
#define SHOVOLOGGER_NDJSONFILESINK_H

#include <fstream>

#include "logger/Sink.h"

namespace logger
{

class NdJsonFileSink : public Sink
{
public:
    explicit NdJsonFileSink(const std::string& filepath);

    void write(const Log& log, const Settings& settings) override;
    void writeHeader(
        const std::string &projectName,
        int argc,
        const char *argv[],
        const BuildInfo& buildInfo,
        const Settings &settings
    ) override;
    void flush() override;
    void close() override;

private:
    std::fstream _file;
};

}

#endif //SHOVOLOGGER_NDJSONFILESINK_H
