message("vtx_python_binding_test")
cmake_minimum_required(VERSION 3.23)
set(PYBIND11_FINDPYTHON ON)

project(vtx_python_binding_test)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_python_binding CONFIG REQUIRED COMPONENTS vtx_python_binding::vtx_python_binding vtx_python_binding::vtx_python_bin)
find_package(Catch2 CONFIG REQUIRED)

set(_VTX_PYTHON_BINDING_TEST_CONAN " ") # So the right target name can be linked
include(cmake/library.cmake)

install(TARGETS vtx_python_binding_test)
install(DIRECTORY ${DIR_PYTHON_SCRIPT} DESTINATION bin)
install(FILES ${PATH_PYTHON_MODULE} DESTINATION bin RENAME ${FILENAME_PYTHON_MODULE})
install(DIRECTORY data DESTINATION bin)