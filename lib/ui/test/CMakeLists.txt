message("vtx_ui_test")
cmake_minimum_required(VERSION 3.23)
project(vtx_ui_test)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_ui CONFIG REQUIRED)
find_package(Catch2 REQUIRED)

add_executable(vtx_ui_test src/main.cpp)

target_link_libraries(vtx_ui_test PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_ui_test PRIVATE vtx_core::vtx_core)
target_link_libraries(vtx_ui_test PRIVATE vtx_renderer::vtx_renderer)
target_link_libraries(vtx_ui_test PRIVATE vtx_io::vtx_io)
target_link_libraries(vtx_ui_test PRIVATE vtx_app::vtx_app)
target_link_libraries(vtx_ui_test PRIVATE vtx_ui::vtx_ui)
target_link_libraries(vtx_ui_test PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(vtx_ui_test)

# Copy dlls in .exe directory
#if(MSVC)
#	add_custom_command(TARGET vtx_ui_test POST_BUILD
#	  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:vtx_ui_test> $<TARGET_FILE_DIR:vtx_ui_test>
#	  COMMAND_EXPAND_LISTS
#	)
#endif()
