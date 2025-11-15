#ifndef SHOVOLOGGER_BUILDINFO_H
#define SHOVOLOGGER_BUILDINFO_H

#include <string>

namespace logger
{

// CMake macros fallback
#ifndef SHLG_BUILD_TYPE
#define SHLG_BUILD_TYPE nullptr
#endif
#ifndef SHLG_BUILD_VERSION
#define SHLG_BUILD_VERSION nullptr
#endif
#ifndef SHLG_COMPILER_ID
#define SHLG_COMPILER_ID nullptr
#endif
#ifndef SHLG_COMPILER_VERSION
#define SHLG_COMPILER_VERSION nullptr
#endif
#ifndef SHLG_COMPILER_FLAGS
#define SHLG_COMPILER_FLAGS nullptr
#endif
#ifndef SHLG_BUILD_SYSTEM
#define SHLG_BUILD_SYSTEM nullptr
#endif

class BuildInfo final
{
public:
    static BuildInfo unknown();
    static BuildInfo fromCMake();

    [[nodiscard]] std::string getType() const { return _type; }
    [[nodiscard]] std::string getVersion() const { return _version; }
    [[nodiscard]] std::string getCompiler() const { return _compiler; }
    [[nodiscard]] std::string getCompilerFlags() const { return _compilerFlags; }
    [[nodiscard]] std::string getBuildSystem() const { return _buildSystem; }

private:
    explicit BuildInfo(
        std::string type,
        std::string version,
        std::string compiler,
        std::string compilerFlags,
        std::string buildSystem
    );

    std::string _type;
    std::string _version;
    std::string _compiler;
    std::string _compilerFlags;
    std::string _buildSystem;
};

}

#endif //SHOVOLOGGER_BUILDINFO_H
