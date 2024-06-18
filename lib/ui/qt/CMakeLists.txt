message("vtx_ui_qt")
cmake_minimum_required(VERSION 3.23)
project(vtx_ui_qt)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_ui CONFIG REQUIRED)
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)

set(_VTX_UI_QT_CONAN " ") # So the right target name can be linked
include("./cmake/library.cmake")

install(TARGETS vtx_ui FILE_SET public_headers DESTINATION include)