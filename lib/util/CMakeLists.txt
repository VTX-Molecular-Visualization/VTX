message("vtx_util")
cmake_minimum_required(VERSION 3.23)
project(vtx_util)

find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(magic_enum REQUIRED)
find_package(cpr REQUIRED)
find_package(Catch2 REQUIRED)

set(_VTX_UTIL_CONAN " ") # So the right target name can be linked
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

install(TARGETS vtx_util FILE_SET public_headers)