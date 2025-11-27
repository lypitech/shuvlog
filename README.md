# 🗒️ Shuvlog

This library is a Logger written in C++ with many features, such as multi-threading, levels and more.

## Getting started

### 1. Build

1. Clone the repository
```shell
git clone git@github.com:lypitech/shuvlog.git
cd shuvlog/
```

2. Build the project
```shell
# Configure and generate build files
cmake -B build/

# Compile the project
cmake --build build/
```

If you want the build to be faster (to use all of your CPU cores), simply add `--parallel` to the options!

Library file will be located in the `build/` folder.  
If you want to build the project in another folder, simply replace `build/` with the folder you want the library to be
built in.
Make sure the folder you specify is empty!

### 2. Link

#### 2.1 CMake

If you use CMake, using this library in your project is really simple.

Simply include the following lines to your `CMakeLists.txt`:
```cmake
add_subdirectory(<library file path>)

target_include_directories(${PROJECT_NAME} PRIVATE
    <library file path>/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    shuvlog
)
```

#### 2.2 Manual linking

If you don't use a proper build system, you can still manually link the library to your project when compiling it.

Simply include the following to your compilation flags:
```
-L <library file path> -l shuvlog
```

## How it works

### 1. Sink

Logs can be outputted anywhere you want, thanks to Sinks.

Sinks, by default, accepts an optional [sink::Settings](#settings) structure as the last parameter.

Here is a list of the sinks implemented by default:

> [!IMPORTANT]
> In formats, everything that is under curly bracket (`{}`) is [configurable](#settings) and so, optional.

> [!IMPORTANT]
> Two sinks can't point to the same output. Why would you print everything more than once in the same file?

---

#### 1.1 `ConsoleSink`
> [!IMPORTANT]  
> Logs that have a level greater than or equal to `kError` will be outputted to the standard error output.

> [!CAUTION]
> Only one instance of `ConsoleSink` can be added. Why would you print everything twice?

__Parameters:__
- *Whether to use colors or not (optional, default to `true`)*
> [!CAUTION]
> Colors does NOT work on versions of Windows prior to Windows 10.

__Header format:__
None.

__Log format:__
```
{{date} time{:milliseconds}} {[{thread_name} {(thread_id)}]} level: message {(source{:line}{:column})}
2025-11-18 16:59:26.006 [MainThread (0x1eda26080)] DEBUG: レイテンシーガール (src/main.cpp:3:16)
```

---

#### 1.2 `LogFileSink`
> [!CAUTION]  
> This kind of Sink should be saved in `.log` files. If it's not the case, a warning will be thrown.

__Parameters:__
- Path of the file where the output will be written. (`const std::string&`)

__Header format:__
```
/*************************************************
|
|   Project            :  R-Type Server (project_name)
|   Version            :  1.0-alpha (project_version)
|   Build type         :  Debug (build_type)
|   Minimum level      :  DEBUG (logger_minimum_level)
|
|   Command            :  ./r-type_server -p 4242 (command_with_arguments)
|   Start time         :  2025-11-18 16:59:26.006 (program_start_time)
|
|   OS                 :  macOS 26.1 (build 25B78) (os_name & kernel_version)
|   Compiler           :  AppleClang 17.0.0.17000404 (compiler & compiler_version)
|   Compilation flags  :  None (compilation_flags)
|   Build system       :  CMake 4.0.2 (build_system & build_system_version)
|
\*************************************************
```

__Log format:__
```
{{date} time{:milliseconds}} {[{thread_name} {(thread_id)}]} level: message {(source{:line}{:column})}
2025-11-18 16:57:25 [MainThread (0x1eda26080)] DEBUG: インフレイション！！ (src/main.cpp:3:16)
```

---

#### 1.3 `JsonFileSink`

> [!CAUTION]  
> This kind of Sink should be saved in `.json` files. If it's not the case, a warning will be thrown.

> [!NOTE]  
> JSON will be properly formatted in the formats below. Note that the Logger will write unformatted JSON (one-lined).

> [!CAUTION]  
> This Sink does not take its assigned settings into account when logging.  
> That's because JSONs are meant to have everything logged into it, as humans won't read it raw. They will use a
> dedicated Log viewer.

__Parameters:__
- Path of the file where the output will be written. (`const std::string&`)

__Header format:__
```json
{
    "projectName": "R-Type Server", // project_name
    "version": "1.0-alpha", // project_version
    "buildType": "Debug", // build_type
    "minimumLevel": "DEBUG", // logger_minimum_level
    "command": "./r-type_server -p 4242", // command_with_arguments
    "startTime": "2025-11-18 16:59:26.006", // program_start_time
    "osName": "macOS 26.1", // os_name
    "kernelVersion": "(build 25B78)", // kernel_version
    "compiler": "AppleClang 17.0.0.17000404", // compiler & compiler_version
    "compilationFlags": "None", // compilation_flags
    "buildSystem": "CMake 4.0.2", // build_system & build_system_version
    "logs": []
}
```

__Log format:__
> [!NOTE]  
> All logs will be put in the `logs` list in the main JSON object.
```json
{
    "timestamp": "2025-11-18 16:59:26.006",
    "level": "DEBUG",
    "thread": {
      "name": "MainThread",
      "id": "0x1eda26080"
    },
    "source": "src/main.cpp",
    "functionName": "int main(const int, const char **)",
    "line": 3,
    "column": 16,
    "message": "成仏させてよ"
}
```

---

#### 1.4 `NdJsonFileSink`
> [!CAUTION]  
> This kind of Sink should be saved in `.json` files. If it's not the case, a warning will be thrown.

This Sink has the same parameters and formats as the [`JsonFileSink`](#13-jsonfilesink), but following the JSON
formatting of [NDJSON](https://docs.mulesoft.com/dataweave/latest/dataweave-formats-ndjson).  

---

#### 1.5 Settings

Sinks are configurable, thanks to the `sink::Settings` structure.

```h
// in namespace logger::sink
struct Settings
{
    bool showTimestamp = true;
    bool showOnlyTime = false;
    bool showMilliseconds = true;

    bool showThreadInfo = true;
    bool showThreadId = true;

    bool showSource = true;
    bool showLineNumber = true;
    bool showColumnNumber = true;
};
```

As said earlier, Sinks can accept a Settings structure, and will automatically adapt their log behaviour accordingly
(depends on the Sink though ; JSON sinks does not.).  
That means you can set different settings for different sinks!

Example:
```cpp
// Most concise settings. No timestamp, no thread info, no source.
logger::sink::Settings conciseSinkSettings{
    .showTimestamp = false,
    .showThreadInfo = false,
    .showSource = false
};

// Only time, no milliseconds.
logger::sink::Settings shortTimeSinkSettings{
    .showOnlyTime = true,
    .showMilliseconds = false
};

Logger#addSink<logger::LogFileSink>("1.log", conciseSinkSettings);
Logger#addSink<logger::LogFileSink>("2.log", shortTimeSinkSettings);

LOG_DEBUG("ゲンダイ");

/**
 *  First sink will output something like that:
 *  DEBUG: ゲンダイ
 *
 *  ... while second sink will output something like that:
 *  23:04:06 [MainThread (0x1eda26080)] DEBUG: ゲンダイ (src/main.cpp:3:16)
 */
```

### 1.6 Levels

Each sink can choose what kind of levels they want to process.

3 modes are available:

| Mode          | Description                                                                       |
|---------------|-----------------------------------------------------------------------------------|
| All           | All levels are processed by the sink (default behavior).                          |
| Explicit      | Only explicitly specified levels are processed by the sink.                       |
| Minimum level | Only levels that are equal or greater than `levelMask` are processed by the sink. |

Example:
```cpp
All levels will be processed by the sink.
Logger#addSink<logger::LogFileSink>(
    "all.log",
    // What is below is facultative, as these are the default values.
    sink::FilterMode::kAll,
    static_cast<uint16_t>(Level::kTraceR1) // When using FilterMode::kAll, this value is ignored.
);

// Logs that have a level greater or equal than Level::kTraceR1 will be processed by the sink.
Logger#addSink<logger::LogFileSink>(
    "minimumLevels.log",
    sink::FilterMode::kMinimumLevel,
    static_cast<uint16_t>(Level::kTraceR1)
);

// Logs that have kTraceR1, kCritical or kFatal will be processed by the sink.
Logger#addSink<logger::LogFileSink>(
    "minimumLevels.log",
    sink::FilterMode::kExplicit,
    Level::kTraceR1 | Level::kCritical | Level::kFatal // Bitwise OR (|) to use multiple values at once.
);
```

## How to use

Before starting the setup, keep in mind that non-static functions in the `Logger` class must be accessed via the
`getInstance()` function.

The only header you will need is `logger/Logger.h`.

### 1. Set up Sinks

Before initializing anything, you need to specify to the Logger where the logs will go.  
Otherwise, they won't go anywhere and the Logger will print an error (`WARNING: Trying to log with no sink.`) each time
you will log something!

For that, just call the `Logger#addSink<T>(...)` function.  
`T` is the Sink class type you want to create  
and `...` the parameters that the Sink class requires (some requires none).

Example:
```c++
Logger::getInstance().addSink<logger::ConsoleSink>();
Logger::getInstance().addSink<logger::LogFileSink>("output.log");
// and so on...
```

### 2. Initialize the Logger

#### 2.1 Build Info

Remember the [headers of Sinks]()? They include information about how the project has been built. This works thanks
to the `logger::BuildInfo` class.

```h
// To save you some reading, this is not the real implementation of the BuildInfo class, but something close.
// Each field can be accessed through getters (eg. getType, getVersion...).
// For more information, read the source code.
struct BuildInfo
{
    static BuildInfo unknown();
    static BuildInfo fromCMake();

    std::string _type;
    std::string _version;
    std::string _compiler;
    std::string _compilerFlags;
    std::string _buildSystem;
}
```

To create an instance of this class, you have two options:
- Either using the static builder `BuildInfo::fromCMake()`, that retrieves the necessary information from CMake
  (if properly [configured](#211-cmake-configuration))
- Or using the static builder `BuildInfo::unknown()`, that returns an instance filled up with "Unknown".

For now, you cannot manually specify your own information. This is being worked on.

##### 2.1.1 CMake configuration

As shown previously, a `BuildInfo` object can be populated using information provided by `CMake`.  
To enable this, you need to add a small configuration snippet to your parent CMake project.

Simply include the following lines:

```cmake
target_compile_definitions(project_name INTERFACE
    SHLG_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    SHLG_BUILD_VERSION="${PROJECT_VERSION}"
    SHLG_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}"
    SHLG_COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}"
    SHLG_COMPILER_FLAGS="${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}"
    SHLG_BUILD_SYSTEM="CMake ${CMAKE_VERSION}"
)
```

#### 2.2 Logger settings

Logger have some settings that can be customized.

```h
// To save you some reading, this is not the real implementation of the logger::Settings class, but something close.
// Each field can be accessed and modified through getters and setters (eg. getMinimumLevel, setFlushIntervalMs...).
// For more information, read the source code.
struct Settings
{
    Level _minimumLevel = Level::kInfo;
    size_t _maxBatchSize = 64;
    int _flushIntervalMs = 250;
};
```

The most important one is the minimum level required for a log to be logged (`_minimumLevel`).
Default is `Level::kInfo`, but you can set something else.

As this Logger is multithreaded, logs are registered into a queue before being flushed. Every `_flushIntervalMs`
milliseconds, the worker removes `_maxBatchSize` logs from the queue and processes them (formatting and flushing).

We recommend you to leave those two variables at their default values.  
If you encounter some synchronization issues with the Logger, you can try to set `_maxBatchSize` to `1` and
`_flushIntervalMs` to `0`.

#### 2.3 Initialization

Now is the time to initialize the Logger, with the function `Logger::initialize`.  
Variables content will be placeholders for demonstration purposes ; you are of course free to use your own.

Example:
```c++
constexpr std::string projectName("R-Type Server");
const auto buildInfo = logger::BuildInfo::fromCMake();
const auto loggerSettings = logger::Settings(logger::Level::kDebug);

Logger::initialize(
    projectName,
    argc, argv,
    buildInfo,
    loggerSettings
);

// You can now properly use the logger.
```

### 3. Usage

Using the Logger is way easier than setting it up.

To log something, just use the available macros:
```c++
LOG_DEBUG("Debug log.");
LOG_INFO("Info log.");
LOG_WARN("Warning log.");
LOG_ERR("Error log.");
LOG_CRIT("Critical log.");
LOG_FATAL("Fatal log.");
```

You can also format your logs, just as in [`std::format`](https://en.cppreference.com/w/cpp/utility/format/format.html):
```c++
constexpr std::string currentMusic("Silly Joke - Instrumental");
LOG_INFO("Hi my name is {}, I'm {} years old and I currently listen to {}.", "Ly", 19, currentMusic);
```

If you want, you can also manually use the `Logger#log` function. But that's too much writing for nothing.

And Logger does the rest! Enjoy logging! :)

Finally, the Logger shuts down by itself when destroyed. That being said, if you want to manually shutdown the Logger,
you can simply call the `Logger#shutdown()` function.

> [!CAUTION]
> If you try to log something with an uninitialized Logger, an error will be written in the standard error output
> (`CAUTION: Logger has been used uninitialized. Make sure you call the initialize() function before performing any
> log.`).

> [!CAUTION]
> As mentionned earlier, if you try to log something with no Sink attached to the Logger, an error will be written in
> the standard error output too (`WARNING: Trying to log with no sink.`).

## Contributing

Contributions are welcome, whether it’s bug fixes, new features, documentation improvements, or ideas to make the
library better. Thank you for taking the time to support the project!

### How to contribute

1. Fork the repository
2. Create a feature branch
3. Commit your modifications with clear messages
4. Push the branch
5. Open a Pull Request describing your changes

### Before opening a PR

**Please make sure:**
- The project successfully builds with CMake on Linux, macOS, and Windows
- Your changes do not introduce warnings (or silence them if intentional)
- All sinks and Logger behaviors remain thread-safe
- You tested your changes with unit tests

### Bug report
Found a bug? Have an idea? Feel free to open an issue.

**When reporting a bug, please include:**
- A minimal code snippet reproducing the bug
- Your compiler version
- Your OS
- Your CMake version

---

## Made with 💜 by [Lysandre B.](https://github.com/shuvlyy) ・ [![wakatime](https://wakatime.com/badge/user/2f50fe6c-0368-4bef-aa01-3a67193b63f8/project/dab4ab21-69ce-4bde-93c7-43f7ab80c99f.svg)](https://wakatime.com/badge/user/2f50fe6c-0368-4bef-aa01-3a67193b63f8/project/dab4ab21-69ce-4bde-93c7-43f7ab80c99f)
###### You can reach me here: [lysandre.boursette@epitech.eu](mailto:lysandre.boursette@epitech.eu)
