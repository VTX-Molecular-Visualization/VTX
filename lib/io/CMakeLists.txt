message("vtx_io")
cmake_minimum_required(VERSION 3.23)
include(cmake/configure-target.cmake)
project(vtx_io)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(chemfiles CONFIG REQUIRED)

add_library(vtx_io)
configureTarget(vtx_io)

file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)
target_sources(vtx_io
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS include FILES ${HEADERS})

target_link_libraries(vtx_io PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_io PRIVATE vtx_core::vtx_core)
target_link_libraries(vtx_io PRIVATE chemfiles::chemfiles)

install(TARGETS vtx_io FILE_SET public_headers)
