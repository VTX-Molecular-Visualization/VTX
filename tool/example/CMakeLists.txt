message("vtx_tool_example")
cmake_minimum_required(VERSION 3.23)
project(vtx_tool_example)

# Internal lib pckages.
find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_ui_qt CONFIG REQUIRED)

# External dependencies.
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
find_package(Catch2 REQUIRED)

# Test libs.
include(CTest)
include(Catch)

# Workaround.
set(_VTX_TOOL_EXAMPLE_CONAN " ")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

# Install target and headers.
install(TARGETS vtx_tool_example FILE_SET public_headers)
