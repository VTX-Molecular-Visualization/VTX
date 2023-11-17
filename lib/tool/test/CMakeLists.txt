message("vtx_tool_test")
cmake_minimum_required(VERSION 3.23)
include(cmake/configure-target.cmake)
project(vtx_tool_test)

find_package(vtx_tool CONFIG REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Catch2 REQUIRED)

add_executable(vtx_tool_test src/main.cpp)
configureTarget(vtx_tool_test)

target_link_libraries(vtx_tool_test PRIVATE vtx_tool::vtx_tool)
target_link_libraries(vtx_tool_test PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(vtx_tool_test)

install(TARGETS vtx_tool_test)
