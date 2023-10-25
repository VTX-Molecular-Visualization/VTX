message("vtx_util")
cmake_minimum_required(VERSION 3.23)
#include(cmake/configure-target.cmake)
project(vtx_util)

find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(magic_enum REQUIRED)
find_package(asio REQUIRED)

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

add_library(vtx_util ${HEADERS} ${SOURCES})
#configureTarget(vtx_util)
target_include_directories(vtx_util PUBLIC include)

target_link_libraries(vtx_util PRIVATE glm::glm)
target_link_libraries(vtx_util PRIVATE spdlog::spdlog)
target_link_libraries(vtx_util PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(vtx_util PRIVATE magic_enum::magic_enum)
target_link_libraries(vtx_util PRIVATE asio::asio)

install(TARGETS vtx_util)