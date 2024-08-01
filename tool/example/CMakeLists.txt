message("vtx_tool_example")
cmake_minimum_required(VERSION 3.23)
project(vtx_tool_example)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(Catch2 REQUIRED)

include(CTest)
include(Catch)

set(_VTX_TOOL_EXAMPLE_CONAN " ")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

install(TARGETS vtx_tool_example FILE_SET public_headers)
