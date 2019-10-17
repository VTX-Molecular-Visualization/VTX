# Declare CMAKE required version.
cmake_minimum_required(VERSION 3.15)

# Project name.
project(VTX)

# Configuration.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Create source paths.
set(PATH_SRC_CORE "src/core")
set(PATH_SRC "src")

# Get all files in /src/ folder (NOT including subfolders).
file(GLOB HPP
    "${PATH_SRC}/*.hpp"
)
file(GLOB CPP
    "${PATH_SRC}/*.cpp"
)

# Get all files in /src/core/ folder (including subfolders).
file(GLOB_RECURSE HPP_CORE
    "${PATH_SRC_CORE}/*.hpp"
)
file(GLOB_RECURSE CPP_CORE
    "${PATH_SRC_CORE}/*.cpp"
)

# Declare executable with source files.
add_executable(VTX ${HPP} ${CPP} ${HPP_CORE} ${CPP_CORE})

# Group source files to create VS filters in solution.
source_group("Headers" FILES ${HPP})
source_group("Sources" FILES ${CPP})
source_group("Headers\\core" FILES ${HPP_CORE})
source_group("Sources\\core" FILES ${CPP_CORE})