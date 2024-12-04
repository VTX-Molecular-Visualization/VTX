message("vtx_python_binding")
cmake_minimum_required(VERSION 3.23)
set(PYBIND11_FINDPYTHON ON)

project(vtx_python_binding)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED )
find_package(vtx_io CONFIG REQUIRED)
find_package(pybind11 REQUIRED)
find_package(EnTT CONFIG REQUIRED)

set(_VTX_PYTHON_BINDING_CONAN " ") # So the right target name can be linked

file(GLOB_RECURSE _VTX_PYTHON_BINDING_CMAKE_DIR_CONTENT "${CMAKE_CURRENT_LIST_DIR}/cmake/*")
message("VTX - _VTX_PYTHON_BINDING_CMAKE_DIR_CONTENT : <${_VTX_PYTHON_BINDING_CMAKE_DIR_CONTENT}>")
include(cmake/library.cmake)

install(TARGETS vtx_python_binding FILE_SET public_headers DESTINATION include)
install(TARGETS vtx_python_bin)
install(DIRECTORY python_script DESTINATION .)