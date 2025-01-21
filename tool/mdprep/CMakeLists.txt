message("vtx_tool_mdprep")
cmake_minimum_required(VERSION 3.23)

project(vtx_tool_mdprep)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_ui_qt CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
find_package(re2 CONFIG REQUIRED)
find_package(Catch2 REQUIRED)
find_package(EnTT CONFIG REQUIRED) # TODO : see <lib/python_binding/cmake/library.cmake> for details

set(_VTX_MDPREP_CONAN " ") # So the right target name can be linked
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

install(TARGETS vtx_tool_mdprep FILE_SET public_headers)