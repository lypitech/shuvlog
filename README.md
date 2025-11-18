# Shovologger

This library is a Logger written in C++ with many features, such as multi-threading, levels and more.

## Getting started

### Installation

1. Clone the repository
```shell
git clone git@github.com:shuvlyy/shovologger.git
cd shovologger/
```

2. Build the project
```shell
# Configure and generate build files
cmake -B build/

# Compile the project
cmake --build build/
```

Library file will be located in the `build/` folder.

## How it works

### Sink

Logs can be outputted anywhere you want, thanks to Sinks.

Sinks, by default, accepts an optional [sink::Settings](#sink-settings) structure as the last parameter.

Here is a list of the sinks implemented by default:

> [!IMPORTANT]
> In formats, everything that is under curly bracket (`{}`) is [configurable](#sink-settings) and so, optional.

<a href="sink-console"></a>
- `ConsoleSink`
  > IMPORTANT:  
  > Logs that have a level greater than or equal to `kError` will be outputted to the standard error output.

  __Parameters:__
  None.

  __Header format:__
  None.

  __Log format:__
  ```
  {{date} time{:milliseconds}} {[{thread_name} {(thread_id)}]} level: message {(source{:line}{:column})}
  2025-11-18 16:59:26.006 [MainThread (0x1eda26080)] DEBUG: レイテンシーガール (src/main.cpp:3:16)
  ```

<a href="sink-logfile"></a>
- `LogFileSink`
  > CAUTION:  
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

<a href="sink-jsonfile"></a>
- `JsonFileSink`
  > CAUTION:  
  > This kind of Sink should be saved in `.json` files. If it's not the case, a warning will be thrown.

  > NOTE:  
  > JSON will be properly formatted in the formats below. Note that the Logger will write unformatted JSON (one-lined).

  > CAUTION:  
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
    "compilerName": "AppleClang 17.0.0.17000404", // compiler & compiler_version
    "compilationFlags": "None", // compilation_flags
    "buildSystem": "CMake 4.0.2", // build_system & build_system_version
    "logs": []
  }
  ```
  
  __Log format:__
  > NOTE:  
  > All logs will be put in the `logs` list in the main JSON object.
  ```json
  {
    "timestamp": "2025-11-18 16:59:26.006",
    "type": "DEBUG",
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

<a href="sink-ndjsonfile"></a>
- `NdJsonFileSink`
  > CAUTION:  
  > This kind of Sink should be saved in `.json` files. If it's not the case, a warning will be thrown.

  This Sink has the same parameters and formats as the [`JsonFileSink`](#sink-jsonfile), but following the JSON
  formatting of [NDJSON](https://docs.mulesoft.com/dataweave/latest/dataweave-formats-ndjson).  


<a href="sink-settings"></a>
#### Settings

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

Remember [the headers of Sinks]()? They include information about how to project has been built. This works thanks
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
  (if properly [configured](#212-cmake-configuration))
- Or using the static builder `BuildInfo::unknown()`, that returns an instance filled up with "Unknown".

For now, you cannot manually specify your own information. This is being worked on.

##### 2.1.2 CMake configuration

TODO

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

## Contributing

## Made with 💜 by [Lysandre B.](https://github.com/shuvlyy) ・ [![wakatime](https://wakatime.com/badge/user/2f50fe6c-0368-4bef-aa01-3a67193b63f8/project/dab4ab21-69ce-4bde-93c7-43f7ab80c99f.svg)](https://wakatime.com/badge/user/2f50fe6c-0368-4bef-aa01-3a67193b63f8/project/dab4ab21-69ce-4bde-93c7-43f7ab80c99f)
###### You can reach me here: [lysandre.boursette@epitech.eu](mailto:lysandre.boursette@epitech.eu)
