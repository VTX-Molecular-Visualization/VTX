message("vtx_io")
cmake_minimum_required(VERSION 3.23)
project(vtx_io)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(chemfiles CONFIG REQUIRED)

file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

add_library(vtx_io STATIC ${HEADERS} ${SOURCES})
target_include_directories(vtx_io PUBLIC include)

target_link_libraries(vtx_io PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_io PRIVATE vtx_core::vtx_core)
target_link_libraries(vtx_io PRIVATE chemfiles::chemfiles)