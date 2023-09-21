message("VTX_CORE")
cmake_minimum_required(VERSION 3.20)
include(../util/cmake/configure-target.cmake)
project(VTX_CORE)

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

# Create library.
add_library(VTX_CORE STATIC ${HEADERS} ${SOURCES})
configureTarget(VTX_CORE)
target_include_directories(VTX_CORE PUBLIC include)

# Link VTX_UTIL.
if(NOT TARGET VTX_UTIL)
	add_subdirectory(../util lib/util)
endif()
target_link_libraries(VTX_CORE PRIVATE VTX_UTIL)