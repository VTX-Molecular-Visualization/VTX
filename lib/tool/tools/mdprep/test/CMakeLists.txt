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

set(_VTX_MDPREP_CONAN " ") # So the right target name can be linked
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")


install(TARGETS vtx_tool_mdprep_test)
install(DIRECTORY data DESTINATION bin)

