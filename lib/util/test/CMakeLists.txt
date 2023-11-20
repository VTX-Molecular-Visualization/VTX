message("vtx_util_test")
cmake_minimum_required(VERSION 3.23)
project(vtx_util_test)

find_package(vtx_util CONFIG REQUIRED)
find_package(Catch2 REQUIRED)

file(GLOB_RECURSE SOURCES src/*)
add_executable(vtx_util_test ${SOURCES})
configure_target(vtx_util_test)

target_link_libraries(vtx_util_test PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_util_test PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(vtx_util_test)

install(TARGETS vtx_util_test)
