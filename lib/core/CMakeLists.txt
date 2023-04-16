message("VTX_CORE")
cmake_minimum_required(VERSION 3.16)
project(VTX_CORE)

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp)

# Create library.
add_library(VTX_CORE STATIC ${HEADERS} ${SOURCES})
target_include_directories(VTX_CORE PRIVATE include)