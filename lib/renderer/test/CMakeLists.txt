message("vtx_renderer_test")
cmake_minimum_required(VERSION 3.23)

project(vtx_renderer_test)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(Catch2 REQUIRED)

include(CTest)
include(Catch)

set(_VTX_RENDERER_TEST_CONAN " ") # So the right target name can be linked
include(cmake/library.cmake)

install(TARGETS vtx_renderer_test)
install(DIRECTORY data DESTINATION bin)