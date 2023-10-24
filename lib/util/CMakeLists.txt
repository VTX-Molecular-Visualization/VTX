message("VTX_UTIL")
cmake_minimum_required(VERSION 3.23)
include(cmake/configure-target.cmake)
project(VTX_UTIL)

# Find packages.
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(magic_enum REQUIRED)

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
find_package(Catch2 REQUIRED)

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