message("vtx_tool_mdprep_test")
cmake_minimum_required(VERSION 3.23)

project(vtx_tool_mdprep_test)

set_property(GLOBAL PROPERTY _REGISTER_DIR_COPY_property "${_REGISTER_DIR_COPY_property}")

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_tool_mdprep CONFIG REQUIRED)
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
find_package(Catch2 REQUIRED)


file(GLOB_RECURSE HEADERS include/*)
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

target_include_directories(vtx_tool_mdprep_test PRIVATE include)

include(CTest)
include(Catch)
catch_discover_tests(vtx_tool_mdprep_test)

install(TARGETS vtx_tool_mdprep_test)
install(DIRECTORY data DESTINATION bin)

vtx_register_build_directory_copy("${CMAKE_CURRENT_LIST_DIR}/data" "data")

vtx_copy_registered_data(vtx_tool_mdprep_test $<TARGET_FILE_DIR:vtx_tool_mdprep_test>)