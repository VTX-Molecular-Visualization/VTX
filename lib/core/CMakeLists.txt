message("vtx_core")
cmake_minimum_required(VERSION 3.23)

project(vtx_core)

find_package(vtx_util CONFIG REQUIRED)

include(cmake/library.cmake)

install(TARGETS vtx_core FILE_SET public_headers)
vtx_install_build_modules()
