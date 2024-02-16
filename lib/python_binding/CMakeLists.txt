message("vtx_python_binding")
cmake_minimum_required(VERSION 3.23)

project(vtx_python_binding)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(pybind11 REQUIRED)

include ("cmake/vtx_python_binding_copy_files.cmake")

add_library(vtx_python_binding)
configure_target(vtx_python_binding)

file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)
target_sources(vtx_python_binding
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS include FILES ${HEADERS})

target_link_libraries(vtx_python_binding PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_python_binding PRIVATE vtx_core::vtx_core)
target_link_libraries(vtx_python_binding PRIVATE vtx_app::vtx_app)
target_link_libraries(vtx_python_binding PRIVATE vtx_io::vtx_io)
target_link_libraries(vtx_python_binding PRIVATE pybind11::pybind11)
target_link_libraries(vtx_python_binding PRIVATE pybind11::embed)

pybind11_add_module(PyTX SHARED "src/python_binding/binding/vtx_module.cpp")

target_link_libraries(PyTX PRIVATE vtx_util::vtx_util)
target_link_libraries(PyTX PRIVATE vtx_core::vtx_core)
target_link_libraries(PyTX PRIVATE vtx_app::vtx_app)
target_link_libraries(PyTX PRIVATE pybind11::pybind11)
target_link_libraries(PyTX PRIVATE pybind11::embed)
target_link_libraries(PyTX PRIVATE vtx_python_binding)

install(TARGETS vtx_python_binding FILE_SET public_headers DESTINATION include)
install(TARGETS PyTX)
install(DIRECTORY python_script DESTINATION .)