message("vtx_ui_qt")
cmake_minimum_required(VERSION 3.23)
project(vtx_ui_qt)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(Qt6 CONFIG COMPONENTS Core REQUIRED)
find_package(Qt6 CONFIG COMPONENTS Gui REQUIRED)
find_package(Qt6 CONFIG COMPONENTS Widgets REQUIRED)
find_package(Qt6 CONFIG COMPONENTS OpenGLWidgets REQUIRED)
find_package(EnTT CONFIG REQUIRED) # TODO : see <lib/python_binding/cmake/library.cmake> for details

set(_VTX_UI_QT_CONAN " ") # So the right target name can be linked
include("./cmake/library.cmake")

install(TARGETS vtx_ui_qt FILE_SET public_headers DESTINATION include)
