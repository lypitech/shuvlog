cmake_minimum_required(VERSION 3.16)

project(shovologger
    VERSION 1.0.0
    DESCRIPTION "Leveled multi-threaded Logger library"
    HOMEPAGE_URL "https://github.com/shuvlyy/shovologger"
    LANGUAGES CXX
)

# ============================================================
#  Compiler settings
# ============================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

# ==============================================================================
#  Multithreaded builds
# ==============================================================================
if(NOT DEFINED CMAKE_BUILD_PARALLEL_LEVEL)
    include(ProcessorCount)
    ProcessorCount(N)
    set(CMAKE_BUILD_PARALLEL_LEVEL ${N})
endif()

# ============================================================
#  Fake parent
# ============================================================
add_library(shlg_parent INTERFACE)

target_compile_definitions(shlg_parent INTERFACE
    SHLG_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    SHLG_BUILD_VERSION="${PROJECT_VERSION}"
    SHLG_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}"
    SHLG_COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}"
    SHLG_COMPILER_FLAGS="${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}"
    SHLG_BUILD_SYSTEM="CMake ${CMAKE_VERSION}"
)

# ============================================================
#  Library target
# ============================================================
add_library(shovologger STATIC
    src/Logger.cpp
    src/OsInfo.cpp
    src/Log.cpp
    src/Timestamp.cpp
    src/BuildInfo.cpp

    src/Sinks/ConsoleSink.cpp
    src/Sinks/LogFileSink.cpp
    src/Sinks/JsonFileSink.cpp
    src/Sinks/NdJsonFileSink.cpp
)

target_link_libraries(shovologger PUBLIC shlg_parent)

# Public headers
target_include_directories(shovologger PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(shovologger PROPERTIES
    OUTPUT_NAME "shovologger"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# ============================================================
#  Tests
# ============================================================
enable_testing()

add_executable(test_shovologger tests/Tester3000.cpp)

target_link_libraries(test_shovologger PRIVATE shovologger)

target_include_directories(test_shovologger PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_test(NAME LoggerTest COMMAND test_shovologger)
