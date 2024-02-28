message("vtx_io_test")
cmake_minimum_required(VERSION 3.23)

project(vtx_io_test)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(Catch2 REQUIRED)

set(_VTX_IO_TEST_CONAN " ") # So the right target name can be linked
include(cmake/library.cmake)

install(TARGETS vtx_io_test)
install(DIRECTORY data DESTINATION bin)