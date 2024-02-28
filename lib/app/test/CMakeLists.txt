message("vtx_app_test")
cmake_minimum_required(VERSION 3.23)

project(vtx_app_test)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(EnTT REQUIRED)
find_package(Catch2 REQUIRED)

include(CTest)
include(Catch)

set(_VTX_APP_TEST_CONAN " ") # So the right target name can be linked
include(cmake/library.cmake)

install(TARGETS vtx_app_test)
install(DIRECTORY data DESTINATION bin)