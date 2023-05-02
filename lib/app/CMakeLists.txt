message("VTX_APP")
cmake_minimum_required(VERSION 3.16)
project(VTX_APP)

# Git dependencies.
set(GIT_URL_CHEMFILES https://github.com/sacraiou/chemfiles.git)
set(GIT_TAG_CHEMFILES e6177423c015ac6ff1a0307b3c3a67c7850d0fe2)
set(GIT_URL_JSON https://github.com/nlohmann/json.git)
set(GIT_TAG_JSON v3.11.2)
set(GIT_URL_MAGIC_ENUM https://github.com/Neargye/magic_enum.git)
set(GIT_TAG_MAGIC_ENUM v0.8.2)

# Fetch third parties.
include(FetchContent)
FetchContent_Declare(chemfiles GIT_REPOSITORY ${GIT_URL_CHEMFILES} GIT_TAG ${GIT_TAG_CHEMFILES})
FetchContent_Declare(nlohmann_json GIT_REPOSITORY ${GIT_URL_JSON} GIT_TAG ${GIT_TAG_JSON})
FetchContent_Declare(magic_enum GIT_REPOSITORY ${GIT_URL_MAGIC_ENUM} GIT_TAG ${GIT_TAG_MAGIC_ENUM})

FetchContent_MakeAvailable(chemfiles nlohmann_json magic_enum)

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

# Create library.
add_library(VTX_APP STATIC ${HEADERS} ${SOURCES})
target_include_directories(VTX_APP PUBLIC include)

# Link VTX_UTIL.
if(NOT TARGET VTX_UTIL)
	add_subdirectory(../util lib/util)
endif()
target_link_libraries(VTX_APP PRIVATE VTX_UTIL)

# Link VTX_RENDERER.
add_subdirectory(../renderer lib/renderer)
target_link_libraries(VTX_APP PRIVATE VTX_RENDERER)


# Link third parties.
target_link_libraries(VTX_APP PUBLIC chemfiles)
target_link_libraries(VTX_APP PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(VTX_APP PUBLIC magic_enum)