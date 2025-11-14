cmake_minimum_required(VERSION 3.16)

project(shovologger
    VERSION 0.0.1
    DESCRIPTION "Leveled multi-threaded Logger library"
    HOMEPAGE_URL "https://github.com/shuvlyy/shovologger"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable extra warnings for GCC/Clang/MSVC
if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

add_library(shovologger STATIC
    src/Logger.cpp
    src/OsInfo.cpp
    src/Settings.cpp
    src/Log.cpp
    src/Timestamp.cpp

    src/Sinks/ConsoleSink.cpp
    src/Sinks/LogFileSink.cpp
    src/Sinks/JsonFileSink.cpp
    src/Sinks/NdJsonFileSink.cpp
)

# === Tests ===
enable_testing()

add_executable(test_shovologger
    tests/Tester3000.cpp
)

# Link the library to the test executable
target_link_libraries(test_shovologger
    PRIVATE shovologger
)

# Make sure the test executable can find the public headers
target_include_directories(test_shovologger
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Register the test with CTest
add_test(NAME LoggerTest COMMAND test_shovologger)

# Expose public headers
target_include_directories(shovologger
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(shovologger PROPERTIES
    OUTPUT_NAME "shovologger"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
