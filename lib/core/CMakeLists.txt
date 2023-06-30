message("VTX_CORE")
cmake_minimum_required(VERSION 3.20)
project(VTX_CORE)

set(GIT_URL_MAGIC_ENUM https://github.com/Neargye/magic_enum.git)
set(GIT_TAG_MAGIC_ENUM v0.8.2)

# Fetch third parties.
include(FetchContent)
FetchContent_Declare(magic_enum GIT_REPOSITORY ${GIT_URL_MAGIC_ENUM} GIT_TAG ${GIT_TAG_MAGIC_ENUM})

FetchContent_MakeAvailable(magic_enum)

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

# Create library.
add_library(VTX_CORE STATIC ${HEADERS} ${SOURCES})
target_include_directories(VTX_CORE PUBLIC include)

# Link VTX_UTIL.
if(NOT TARGET VTX_UTIL)
	add_subdirectory(../util lib/util)
endif()
target_link_libraries(VTX_CORE PRIVATE VTX_UTIL)

# Link third parties.
target_link_libraries(VTX_CORE PUBLIC magic_enum)