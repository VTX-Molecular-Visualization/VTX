message("vtx_ui")
cmake_minimum_required(VERSION 3.23)
project(vtx_ui)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_python_binding CONFIG REQUIRED COMPONENTS vtx_python_binding::vtx_python_binding vtx_python_binding::PyTX)
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)

set(_VTX_UI_CONAN " ") # So the right target name can be linked
include("./cmake/library.cmake")

install(TARGETS vtx_ui FILE_SET public_headers DESTINATION include)