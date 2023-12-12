message("vtx_tool_mdprep_test")
cmake_minimum_required(VERSION 3.23)

project(vtx_tool_mdprep_test)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_tool_mdprep CONFIG REQUIRED)
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
find_package(Catch2 REQUIRED)


file(GLOB_RECURSE SOURCES src/*)

add_executable(vtx_tool_mdprep_test ${SOURCES})
configure_target(vtx_tool_mdprep_test)

target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_app::vtx_app)
target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_core::vtx_core)
target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_tool_mdprep::vtx_tool_mdprep)
target_link_libraries(vtx_tool_mdprep_test PRIVATE Qt6::Core)			 # I feel weird about linked lib not being 
target_link_libraries(vtx_tool_mdprep_test PRIVATE Qt6::Gui)			 # carried through packages, forcing me
target_link_libraries(vtx_tool_mdprep_test PRIVATE Qt6::Widgets)		 # to link Qt6 explicitly. I don't like it
target_link_libraries(vtx_tool_mdprep_test PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(vtx_tool_mdprep_test PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(vtx_tool_mdprep_test)

add_custom_target(vtx_tool_mdprep_test_copy_data ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:vtx_tool_mdprep_test>/data)
add_custom_target(vtx_tool_mdprep_test_copy_gromacs_dll ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different ${VTX_GROMACS_DLL_PATH} $<TARGET_FILE_DIR:vtx_tool_mdprep_test>)
add_dependencies(vtx_tool_mdprep_test vtx_tool_mdprep_test_copy_data)

install(TARGETS vtx_tool_mdprep_test)
install(DIRECTORY data DESTINATION bin)