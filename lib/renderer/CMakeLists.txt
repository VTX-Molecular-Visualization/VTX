message("vtx_renderer")
cmake_minimum_required(VERSION 3.23)

project(vtx_renderer)

find_package(vtx_util CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

add_library(vtx_renderer)
configure_target(vtx_renderer)

file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)
file(GLOB_RECURSE SHADERS shaders/*)
target_sources(vtx_renderer
	PRIVATE ${SOURCES}
	PRIVATE ${SHADERS}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS include FILES ${HEADERS})

target_link_libraries(vtx_renderer PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_renderer PRIVATE glad::glad)

install(TARGETS vtx_renderer FILE_SET public_headers DESTINATION include)
install(DIRECTORY shaders DESTINATION .)