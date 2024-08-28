message("vtx_io")
cmake_minimum_required(VERSION 3.23)
project(vtx_io)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(chemfiles CONFIG REQUIRED)
find_package(Catch2 REQUIRED)

include(CTest)
include(Catch)

set(_VTX_IO_CONAN " ") # So the right target name can be linked
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

install(TARGETS vtx_io FILE_SET public_headers)
