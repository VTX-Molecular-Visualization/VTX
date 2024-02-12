message("vtx_core")
cmake_minimum_required(VERSION 3.23)

project(vtx_core)

find_package(vtx_util CONFIG REQUIRED)

add_library(vtx_core)
configure_target(vtx_core)

file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)
target_sources(vtx_core
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS include FILES ${HEADERS})

target_link_libraries(vtx_core PRIVATE vtx_util::vtx_util)

install(TARGETS vtx_core FILE_SET public_headers)