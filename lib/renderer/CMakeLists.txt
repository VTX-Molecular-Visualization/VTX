message("vtx_renderer")
cmake_minimum_required(VERSION 3.23)
include(cmake/configure-target.cmake)
project(vtx_renderer)

find_package(vtx_util CONFIG REQUIRED)

add_library(vtx_renderer)
configureTarget(vtx_renderer)

file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)
file(GLOB_RECURSE VENDORS vendor/*)
target_sources(vtx_renderer
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS include FILES ${HEADERS}
	PUBLIC FILE_SET public_vendors TYPE HEADERS BASE_DIRS vendor FILES ${VENDORS})

target_link_libraries(vtx_renderer PRIVATE vtx_util::vtx_util)

install(TARGETS vtx_renderer FILE_SET public_headers FILE_SET public_vendors)