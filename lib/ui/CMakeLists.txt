message("vtx_ui")
cmake_minimum_required(VERSION 3.23)
project(vtx_ui)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)

set(_VTX_UI_CONAN " ")
include("./cmake/library.cmake")

install(TARGETS vtx_ui FILE_SET public_headers DESTINATION include)