message("vtx_renderer_test")
cmake_minimum_required(VERSION 3.23)
project(vtx_renderer_test)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(Catch2 REQUIRED)

add_executable(vtx_renderer_test src/main.cpp)

target_link_libraries(vtx_renderer_test PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_renderer_test PRIVATE vtx_renderer::vtx_renderer)
target_link_libraries(vtx_renderer_test PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(vtx_renderer_test)