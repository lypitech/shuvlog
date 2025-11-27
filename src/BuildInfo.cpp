#include <utility>

#include "logger/BuildInfo.h"

namespace logger
{

static const std::string ukw = "Unknown";

BuildInfo::BuildInfo(
    std::string type,
    std::string version,
    std::string compiler,
    std::string compilerFlags,
    std::string buildSystem
)
    : _type(std::move(type))
    , _version(std::move(version))
    , _compiler(std::move(compiler))
    , _compilerFlags(std::move(compilerFlags))
    , _buildSystem(std::move(buildSystem))
{}

BuildInfo BuildInfo::unknown()
{
    return BuildInfo(ukw, ukw, ukw, ukw, ukw);
}

BuildInfo BuildInfo::fromCMake()
{
    auto get = [](auto macro, const std::string& fallback = ukw) {
        return std::string(macro ? macro : fallback);
    };

    std::string type = get(SHLG_BUILD_TYPE);
    const std::string version = get(SHLG_BUILD_VERSION);
    std::string flags = get(SHLG_COMPILER_FLAGS);
    const std::string buildSystem = get(SHLG_BUILD_SYSTEM);

    const std::string compiler =
        get(SHLG_COMPILER_ID) + " " +
        get(SHLG_COMPILER_VERSION, "");

    if (type.empty()) {
        type = "Unknown";
    }

    if (flags == " ") {
        flags = "None";
    }

    return BuildInfo(type, version, compiler, flags, buildSystem);
}

}
