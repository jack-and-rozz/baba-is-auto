# CMake version
cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)

# Include cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

# Declare project
project(baba-is-auto)

# Set output directories
set(DEFAULT_CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Set enable output of compile commands during generation
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Includes
include_directories(Includes)
include_directories(Extensions/BabaPython/Includes)
include_directories(Libraries)
# include_directories(Libraries/doctest/doctest)
include_directories(Libraries/pybind11/include)
include_directories(Libraries/random/include)

# Compile options
include(CMake/CompileOptions.cmake)

# Build type - Release by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Overrides
set(CMAKE_MACOSX_RPATH ON)

# Set resources
set(MAPS_DIR ${PROJECT_SOURCE_DIR}/Resources/Maps)

# Project modules
# add_subdirectory(Libraries/doctest)
add_subdirectory(Sources/baba-is-auto)
# add_subdirectory(Tests/UnitTests)

# Code coverage - Debug only
# NOTE: Code coverage results with an optimized (non-Debug) build may be misleading
option(BUILD_COVERAGE "Build code coverage" OFF)
if (CMAKE_BUILD_TYPE MATCHES Debug AND CMAKE_COMPILER_IS_GNUCXX AND BUILD_COVERAGE)
    include(CodeCoverage)
    setup_target_for_coverage(${PROJECT_NAME}_coverage UnitTests coverage)
endif()

add_subdirectory(Libraries/pybind11)
if (BUILD_FROM_PIP)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DEFAULT_CMAKE_LIBRARY_OUTPUT_DIRECTORY})
endif()
add_subdirectory(Extensions/BabaPython)
