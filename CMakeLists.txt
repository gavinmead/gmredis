cmake_minimum_required(VERSION 3.28)
project(gmredis
    VERSION 0.1.0
    DESCRIPTION "A Redis clone for learning modern C++23"
    LANGUAGES CXX
)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type defaults to Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Options
option(GMREDIS_BUILD_TESTS "Build tests" ON)
option(GMREDIS_BUILD_SERVER "Build Redis server" ON)
option(GMREDIS_BUILD_CLIENT "Build Redis client" ON)

# Compiler warnings
add_library(gmredis_warnings INTERFACE)
target_compile_options(gmredis_warnings INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
        -Wall -Wextra -Wpedantic -Werror
        -Wconversion -Wsign-conversion
        -Wnon-virtual-dtor -Wold-style-cast
        -Wcast-align -Wunused -Woverloaded-virtual
        -Wnull-dereference -Wdouble-promotion
        -Wformat=2 -Wimplicit-fallthrough
    >
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4 /WX /permissive-
    >
)

# Conan package manager integration
find_package(GTest REQUIRED)
find_package(Catch2 REQUIRED)

# Core library
add_subdirectory(lib/gmredis)

# Executables
if(GMREDIS_BUILD_SERVER)
    add_subdirectory(src/server)
endif()

if(GMREDIS_BUILD_CLIENT)
    add_subdirectory(src/client)
endif()

# Tests
if(GMREDIS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
