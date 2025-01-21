message("vtx_util")
cmake_minimum_required(VERSION 3.23)
project(vtx_util)

find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(stb CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

set(_VTX_UTIL_CONAN " ") # So the right target name can be linked
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

install(TARGETS vtx_util FILE_SET public_headers)