message("vtx_core")
cmake_minimum_required(VERSION 3.23)

project(vtx_core)

find_package(vtx_util CONFIG REQUIRED)

set(_VTX_CORE_CONAN " ") # So the right target name can be linked
include(cmake/library.cmake)

install(TARGETS vtx_core FILE_SET public_headers)