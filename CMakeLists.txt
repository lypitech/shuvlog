cmake_minimum_required(VERSION 3.16)

project(shuvlog
    VERSION 1.2.1
    DESCRIPTION "Asynchronous, thread-safe, singleton logging engine library"
    HOMEPAGE_URL "https://github.com/lypitech/shuvlog"
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
    add_compile_options(-fmacro-prefix-map=${CMAKE_SOURCE_DIR}=.)
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
add_library(${PROJECT_NAME} STATIC
    src/Logger.cpp
    src/OsInfo.cpp
    src/Log.cpp
    src/Timestamp.cpp
    src/BuildInfo.cpp
    src/Sink.cpp
    src/FileSink.cpp

    src/Sinks/ConsoleSink.cpp
    src/Sinks/LogFileSink.cpp
    src/Sinks/JsonFileSink.cpp
    src/Sinks/NdJsonFileSink.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC shlg_parent)

# Public headers
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# ============================================================
#  Tests
# ============================================================
enable_testing()

add_executable(test_shuvlog tests/Tester3000.cpp)

target_link_libraries(test_shuvlog PRIVATE shuvlog)

target_include_directories(test_shuvlog PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_test(NAME LoggerTest COMMAND test_shuvlog)
