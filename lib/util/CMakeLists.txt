message("VTX_UTIL")
cmake_minimum_required(VERSION 3.23)
#include(cmake/configure-target.cmake)
project(VTX_UTIL)

# Find packages.
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(magic_enum REQUIRED)
find_package(asio REQUIRED)

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

# Create library.
add_library(VTX_UTIL ${HEADERS} ${SOURCES})
#configureTarget(VTX_UTIL)
target_include_directories(VTX_UTIL PUBLIC include)

# Link third parties.
target_link_libraries(VTX_UTIL PUBLIC glm::glm)
target_link_libraries(VTX_UTIL PUBLIC spdlog::spdlog)
target_link_libraries(VTX_UTIL PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(VTX_UTIL PUBLIC magic_enum::magic_enum)

set_target_properties(VTX_UTIL PROPERTIES PUBLIC_HEADER HEADERS)

# Install.
install(TARGETS VTX_UTIL)