message("vtx_python_binding")
cmake_minimum_required(VERSION 3.23)

project(vtx_python_binding)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(pybind11 REQUIRED)

set(_VTX_PYTHON_BINDING_CONAN " ") # So the right target name can be linked
include(cmake/library.cmake)

install(TARGETS vtx_python_binding FILE_SET public_headers DESTINATION include)
install(TARGETS PyTX)
install(DIRECTORY python_script DESTINATION .)