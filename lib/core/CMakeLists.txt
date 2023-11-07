message("vtx_core")
cmake_minimum_required(VERSION 3.23)
project(vtx_core)

find_package(vtx_util CONFIG REQUIRED)

file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

add_library(vtx_core STATIC ${HEADERS} ${SOURCES})
target_include_directories(vtx_core PUBLIC include)

target_link_libraries(vtx_core PRIVATE vtx_util::vtx_util)