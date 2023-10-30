message("vtx_tool")
cmake_minimum_required(VERSION 3.23)
project(vtx_tool)

find_package(vtx_tool CONFIG REQUIRED)
find_package(Catch2 REQUIRED)

add_executable(vtx_tool_test src/main.cpp)

target_link_libraries(vtx_ui_test PRIVATE vtx_tool::vtx_tool)
target_link_libraries(vtx_tool_test PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(vtx_ui_test)

