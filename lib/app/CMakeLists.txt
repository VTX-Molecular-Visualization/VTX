message("vtx_app")
cmake_minimum_required(VERSION 3.23)
project(vtx_app)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

set(_VTX_APP_CONAN " ") # So the right target name can be linked
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

install(TARGETS vtx_app poneyponey FILE_SET public_headers DESTINATION include)
