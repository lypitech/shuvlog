#include <filesystem>

#include "logger/FileSink.h"
#include "logger/Exceptions/BadFileExtension.h"
#include "logger/Exceptions/BadRecommendedExtension.h"
#include "logger/Exceptions/CouldNotOpenFile.h"

namespace logger
{

FileSink::FileSink(
    const std::string& filepath,
    const std::string& extensionName,
    const std::string& recommendedExtension,
    const sink::Settings settings
)
    : FileSink(
        filepath,
        extensionName,
        recommendedExtension,
        sink::FilterMode::kAll,
        0xFFFF,
        settings
    )
{}

FileSink::FileSink(
    const std::string &filepath,
    const std::string &extensionName,
    const std::string &recommendedExtension,
    sink::FilterMode filterMode,
    uint16_t levelMask,
    sink::Settings settings
)
    : Sink(filterMode, levelMask, settings)
    , _absoluteFilepath(std::filesystem::absolute(filepath).generic_string())
{
    if (!recommendedExtension.starts_with(".") || recommendedExtension.length() < 2) {
        throw exception::BadRecommendedExtension(recommendedExtension);
    }

    if (!_absoluteFilepath.ends_with(recommendedExtension)) {
        throw exception::BadFileExtension(extensionName, recommendedExtension);
    }

    _file.open(_absoluteFilepath, std::ios::in | std::ios::out | std::ios::trunc);
    if (!_file.is_open()) {
        throw exception::CouldNotOpenFile(filepath);
    }
}

}
