message("vtx_tool_test")
cmake_minimum_required(VERSION 3.23)
project(vtx_tool_test)

find_package(vtx_tool CONFIG REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Catch2 REQUIRED)

include(CTest)
include(Catch)

set(_VTX_TOOL_TEST_CONAN " ")
include(cmake/library.cmake)

install(TARGETS vtx_tool_test)
