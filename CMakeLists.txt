cmake_minimum_required(VERSION 3.16)

project(shuvlog
    VERSION 1.2.2
    DESCRIPTION "Asynchronous, thread-safe, singleton logging engine library"
    HOMEPAGE_URL "https://github.com/lypitech/shuvlog"
    LANGUAGES CXX
)

# --- Options ---
option(SHUVLOG_BUILD_TESTS "Build the test suite" OFF)

# --- Sources / Headers ---
add_library(${PROJECT_NAME} STATIC
    src/Logger.cpp
    src/OsInfo.cpp
    src/Log.cpp
    src/Timestamp.cpp
    src/Sink.cpp
    src/FileSink.cpp

    src/Sinks/ConsoleSink.cpp
    src/Sinks/LogFileSink.cpp
    src/Sinks/JsonFileSink.cpp
    src/Sinks/NdJsonFileSink.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# --- Output ---
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# --- Compiler settings ---
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Werror -pedantic
        -fmacro-prefix-map=${CMAKE_SOURCE_DIR}=.
    )
endif()

# --- Tests ---
if(SHUVLOG_BUILD_TESTS OR PROJECT_IS_TOP_LEVEL)
    enable_testing()
    add_executable(test_shuvlog tests/Tester3000.cpp)
    target_link_libraries(test_shuvlog PRIVATE ${PROJECT_NAME})
    target_include_directories(test_shuvlog PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_compile_definitions(test_shuvlog PRIVATE
        SHLG_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
        SHLG_BUILD_VERSION="${PROJECT_VERSION}"
        SHLG_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}"
        SHLG_COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}"
        SHLG_COMPILER_FLAGS="${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}"
        SHLG_BUILD_SYSTEM="CMake ${CMAKE_VERSION}"
    )

    add_test(NAME LoggerTest COMMAND test_shuvlog)
endif()
