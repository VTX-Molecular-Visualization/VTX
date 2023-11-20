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

file(GLOB_RECURSE SOURCES src/*)
add_executable(vtx_app_test ${SOURCES})

target_link_libraries(vtx_app_test PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_app_test PRIVATE vtx_renderer::vtx_renderer)
target_link_libraries(vtx_app_test PRIVATE vtx_core::vtx_core)
target_link_libraries(vtx_app_test PRIVATE vtx_io::vtx_io)
target_link_libraries(vtx_app_test PRIVATE vtx_app::vtx_app)
target_link_libraries(vtx_app_test PRIVATE EnTT::EnTT)
target_link_libraries(vtx_app_test PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(vtx_app_test)

# Copy data.
add_custom_target(vtx_app_test_copy_data ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:vtx_app_test>/data)
add_dependencies(vtx_app_test vtx_app_test_copy_data)

install(TARGETS vtx_app_test)
install(DIRECTORY data DESTINATION bin)