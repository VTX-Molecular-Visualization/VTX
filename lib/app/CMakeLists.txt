message("vtx_app")
cmake_minimum_required(VERSION 3.23)
include(../util/cmake/configure-target.cmake)
project(vtx_app)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(EnTT REQUIRED)

file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

add_library(vtx_app STATIC ${HEADERS} ${SOURCES})
configureTarget(vtx_app)
target_include_directories(vtx_app PUBLIC include)

target_link_libraries(vtx_app PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_app PRIVATE vtx_renderer::vtx_renderer)
target_link_libraries(vtx_app PRIVATE vtx_core::vtx_core)
target_link_libraries(vtx_app PRIVATE vtx_io::vtx_io)
target_link_libraries(vtx_app PRIVATE EnTT::EnTT)