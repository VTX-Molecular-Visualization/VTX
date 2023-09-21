message("VTX_UTIL")
cmake_minimum_required(VERSION 3.20)
include(cmake/configure-target.cmake)
project(VTX_UTIL)

# Git dependencies.
set(GIT_URL_GLM https://github.com/g-truc/glm.git)
set(GIT_TAG_GLM 0.9.9.8)
set(GIT_URL_SPDLOG https://github.com/gabime/spdlog.git)
set(GIT_TAG_SPDLOG v1.11.0)
set(GIT_URL_JSON https://github.com/nlohmann/json.git)
set(GIT_TAG_JSON v3.11.2)
set(GIT_URL_MAGIC_ENUM https://github.com/Neargye/magic_enum)
set(GIT_TAG_MAGIC_ENUM v0.9.3)

# Fetch third parties.
include(FetchContent)
FetchContent_Declare(glm GIT_REPOSITORY ${GIT_URL_GLM} GIT_TAG ${GIT_TAG_GLM})
FetchContent_Declare(spdlog GIT_REPOSITORY ${GIT_URL_SPDLOG} GIT_TAG ${GIT_TAG_SPDLOG})
FetchContent_Declare(nlohmann_json GIT_REPOSITORY ${GIT_URL_JSON} GIT_TAG ${GIT_TAG_JSON})
FetchContent_Declare(magic_enum GIT_REPOSITORY ${GIT_URL_MAGIC_ENUM} GIT_TAG ${GIT_TAG_MAGIC_ENUM})

FetchContent_MakeAvailable(glm spdlog nlohmann_json magic_enum)

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

# Create library.
add_library(VTX_UTIL STATIC ${HEADERS} ${SOURCES})
configureTarget(VTX_UTIL)
target_include_directories(VTX_UTIL PUBLIC include)

# Link third parties.
target_link_libraries(VTX_UTIL PUBLIC glm::glm)
target_link_libraries(VTX_UTIL PUBLIC spdlog::spdlog)
target_link_libraries(VTX_UTIL PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(VTX_UTIL PUBLIC magic_enum::magic_enum)

# Tests.
set(GIT_URL_CATCH2 https://github.com/catchorg/Catch2.git)
set(GIT_TAG_CATCH2 v3.3.2)

FetchContent_Declare(Catch2 GIT_REPOSITORY ${GIT_URL_CATCH2} GIT_TAG ${GIT_TAG_CATCH2})
FetchContent_MakeAvailable(Catch2)

# Catch2 configuration.
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

# Create executable.
add_executable(VTX_UTIL_TEST test/main.cpp)
configureTarget(VTX_UTIL_TEST)

# Link third parties.
target_link_libraries(VTX_UTIL_TEST PRIVATE Catch2::Catch2WithMain)

# Link VTX_UTIL.
target_link_libraries(VTX_UTIL_TEST PRIVATE VTX_UTIL)

# Catch2.
include(CTest)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	catch_discover_tests(VTX_UTIL_TEST)
endif()