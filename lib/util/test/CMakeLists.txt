message("vtx_util_test")
cmake_minimum_required(VERSION 3.23)
project(vtx_util_test)

find_package(vtx_util CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Catch2 REQUIRED)

include(CTest)
include(Catch)

set(_VTX_UTIL_TEST_CONAN " ") # So the right target name can be linked
include(cmake/library.cmake)

install(TARGETS vtx_util_test)
