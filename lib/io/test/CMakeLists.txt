message("vtx_io_test")
cmake_minimum_required(VERSION 3.23)
include(../cmake/configure-target.cmake)
project(vtx_io_test)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(Catch2 REQUIRED)

add_executable(vtx_io_test src/main.cpp)
configureTarget(vtx_io_test)

target_link_libraries(vtx_io_test PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_io_test PRIVATE vtx_core::vtx_core)
target_link_libraries(vtx_io_test PRIVATE vtx_io::vtx_io)
target_link_libraries(vtx_io_test PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(vtx_io_test)

add_custom_target(vtx_io_test_copy_data ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:vtx_io_test>/data)
add_dependencies(vtx_io_test vtx_io_test_copy_data)