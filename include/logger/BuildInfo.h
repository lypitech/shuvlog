#ifndef SHUVLOG_BUILDINFO_H
#define SHUVLOG_BUILDINFO_H

#include <string>

namespace logger
{

static const std::string ukw = "Unknown";

/**
 * @class   BuildInfo
 * @brief   Build-time metadata about project build.
 *
 * This class stores information about the build configuration, compiler,
 * compiler flags, and the build system used to produce the executable or
 * library.
 * This class can only be constructed through available factories, and its
 * fields are read-only.
 *
 * Instances of this class are used by Logger sinks to write a structured
 * startup header that describes the environment in which the application
 * was built.
 */
class BuildInfo final
{
public:
    /**
     * @return  BuildInfo class populated with "Unknown" strings.
     */
    static BuildInfo unknown() {
        return BuildInfo(ukw, ukw, ukw, ukw, ukw);
    }

    /**
     * @return  An automatically populated BuildInfo class with
     *          information that CMake provides through CMake-generated
     *          macros (e.g. @code SHLG_BUILD_TYPE@endcode)
     */
    static BuildInfo fromCMake() {
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

        auto get = [](const char* macro, const std::string& fallback = ukw) {
            return std::string((macro != nullptr) ? macro : fallback);
        };

        std::string type = get(SHLG_BUILD_TYPE);
        const std::string version = get(SHLG_BUILD_VERSION);
        std::string flags = get(SHLG_COMPILER_FLAGS);
        const std::string buildSystem = get(SHLG_BUILD_SYSTEM);

        const std::string compiler =
            get(SHLG_COMPILER_ID) + " " +
            get(SHLG_COMPILER_VERSION, "");

        if (type.empty()) {
            type = ukw;
        }

        if (flags == " ") {
            flags = "None";
        }

        return BuildInfo(type, version, compiler, flags, buildSystem);
    }

    /// @return The CMake build type (Debug, Release, etc.).
    [[nodiscard]] std::string getType() const { return _type; }

    /// @return The project version defined at build time.
    [[nodiscard]] std::string getVersion() const { return _version; }

    /// @return The name and version of the compiler used to build the project.
    [[nodiscard]] std::string getCompiler() const { return _compiler; }

    /// @return The compiler flags that were active during the build.
    [[nodiscard]] std::string getCompilerFlags() const { return _compilerFlags; }

    /// @return The name and version of the build system (e.g., "CMake 3.16").
    [[nodiscard]] std::string getBuildSystem() const { return _buildSystem; }

private:
    /**
     * This constructor is private to force users to create a BuildInfo
     * class with the @code unknown()@endcode and @code fromCMake()@endcode
     * factory methods.
     *
     * @param   type            The build type string.
     * @param   version         The project version string.
     * @param   compiler        The compiler identifier and version.
     * @param   compilerFlags   The active compiler flags.
     * @param   buildSystem     The build system name and version.
     */
    explicit BuildInfo(
        std::string type,
        std::string version,
        std::string compiler,
        std::string compilerFlags,
        std::string buildSystem
    )   : _type(std::move(type))
        , _version(std::move(version))
        , _compiler(std::move(compiler))
        , _compilerFlags(std::move(compilerFlags))
        , _buildSystem(std::move(buildSystem)) {}

    std::string _type;
    std::string _version;
    std::string _compiler;
    std::string _compilerFlags;
    std::string _buildSystem;
};

}

#endif //SHUVLOG_BUILDINFO_H
