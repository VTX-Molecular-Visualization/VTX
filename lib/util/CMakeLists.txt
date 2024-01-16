message("vtx_util")
cmake_minimum_required(VERSION 3.23)
include(cmake/configure_target.cmake)
include(cmake/copy_registration.cmake)
project(vtx_util)

find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(magic_enum REQUIRED)
find_package(asio REQUIRED)

add_library(vtx_util)
configure_target(vtx_util)

file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)
target_sources(vtx_util
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS include FILES ${HEADERS})

target_link_libraries(vtx_util PRIVATE glm::glm)
target_link_libraries(vtx_util PRIVATE spdlog::spdlog)
target_link_libraries(vtx_util PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(vtx_util PRIVATE magic_enum::magic_enum)
target_link_libraries(vtx_util PRIVATE asio::asio)

install(TARGETS vtx_util FILE_SET public_headers)