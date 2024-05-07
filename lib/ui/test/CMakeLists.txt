message("vtx_ui_test")
cmake_minimum_required(VERSION 3.23)

project(vtx_ui_test)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_python_binding CONFIG REQUIRED COMPONENTS vtx_python_binding::vtx_python_binding vtx_python_binding::PyTX)
find_package(pybind11 REQUIRED)
find_package(vtx_ui CONFIG REQUIRED)
find_package(Catch2 REQUIRED)

set(_VTX_UI_TEST_CONAN " ") # So the right target name can be linked
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

install(TARGETS vtx_ui_test)
install(DIRECTORY data DESTINATION bin)
#TODO: install dll too.