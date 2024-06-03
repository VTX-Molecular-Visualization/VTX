message("vtx_tool_test")
cmake_minimum_required(VERSION 3.23)

project(vtx_tool_test)

find_package(vtx_tool CONFIG REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Catch2 REQUIRED)

add_executable(vtx_tool_test src/main.cpp)
configure_target(vtx_tool_test)

target_link_libraries(vtx_tool_test PRIVATE vtx_tool::vtx_tool)
target_link_libraries(vtx_tool_test PRIVATE Catch2::Catch2WithMain)

target_compile_definitions(vtx_tool_test PRIVATE VTX_RENDERER_NO_OPENGL)

include(CTest)
include(Catch)
set(CMAKE_CATCH_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
catch_discover_tests(vtx_tool_test)

install(TARGETS vtx_tool_test)
