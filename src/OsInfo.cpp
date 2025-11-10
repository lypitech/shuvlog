#include "logger/OsInfo.h"

#include <array>
#include <format>

static std::string runCommand(const std::string& command)
{
    std::array<char, 128> buf{};
    std::string res{};
#if defined(__APPLE__) || defined(__linux__)
    const std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
#elif defined(_WIN32)
    const std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
#else
    return {};
#endif

    if (!pipe) {
        return {};
    }
    while (fgets(buf.data(), buf.size(), pipe.get()) != nullptr) {
        res.append(buf.data());
    }
    if (!res.empty() && res.back() == '\n') {
        res.pop_back();
    }
    return res;
}

std::string osname()
{
#if defined(__APPLE__)
    std::string name = runCommand("sw_vers -productName");
    std::string version = runCommand("sw_vers -productVersion");

    if (!name.empty() && !version.empty()) {
        return std::format("{} {}", name, version);
    }
    return "macOS";
#endif
#if defined(__linux__)
    std::ifstream osRelease("/etc/os-release");
    std::string line;

    while (std::getline(osRelease, line)) {
        if (line.substr(0, 5) == "NAME=") {
            std::string res = line.substr(6);
            res.pop_back();
            return res;
        }
    }
    return "Linux";
#endif
#if defined(_WIN32)
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        std::array<char, 256> productName{};
        DWORD size = sizeof(productName);

        if (RegGetValueA(hKey, nullptr, "ProductName",
                         RRF_RT_REG_SZ, nullptr, productName,
                         &size) == ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return std::string(productName);
        }
        RegCloseKey(hKey);
    }
    return "Windows";
#endif
}

std::string kernelver()
{
#if defined(__APPLE__)
    std::string buildver = runCommand("sw_vers -buildVersion");

    if (!buildver.empty()) {
        return std::format("(build {})", buildver);
    }
    return "(unknown build)";
#endif
#if defined(__linux__)
    utsname uname_data{};

    if (uname(&uname_data) == 0) {
        return std::format("(build {})", uname_data.release);
    }
    return "(unknown build)";
#endif
#if defined(_WIN32)
    OSVERSIONINFOEXA osvi{};
    osvi.dwOSVersionInfoSize = sizeof(osvi);

    if (RtlGetVersion != nullptr) {
        using RtlGetVersionFunc = LONG(WINAPI*)(PRTL_OSVERSIONINFOW);
        auto ntdll = GetModuleHandleW(L"ntdll.dll");
        auto rtlGetVersion = reinterpret_cast<RtlGetVersionFunc>(GetProcAddress(ntdll, "RtlGetVersion"));

        if (rtlGetVersion) {
            RTL_OSVERSIONINFOW ver{};
            ver.dwOSVersionInfoSize = sizeof(ver);
            rtlGetVersion(&ver);
            return std::format("(build {}.{}.{})", ver.dwMajorVersion, ver.dwMinorVersion, ver.dwBuildNumber);
        }
    }
    return "(unknown build)";
#endif
}
