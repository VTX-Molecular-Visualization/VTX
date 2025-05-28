message("vtx_python_binding")
cmake_minimum_required(VERSION 3.23)
set(PYBIND11_FINDPYTHON ON)

project(vtx_python_binding)


find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

set(_VTX_PYTHON_BINDING_CONAN " ") # So the right target name can be linked
include(cmake/library.cmake)

# Valgrind
#SET(USE_VALGRIND "1")
if (DEFINED USE_VALGRIND)
	message("VTX - Setting up valgrind ...")
	find_program( MEMORYCHECK_COMMAND valgrind )
	set( MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full" )
	set( MEMORYCHECK_SUPPRESSIONS_FILE ${CMAKE_CURRENT_BINARY_DIR}/valgrind_suppress.txt )
endif()
# !Valgrind

install(TARGETS vtx_python_binding FILE_SET public_headers DESTINATION include)
install(TARGETS vtx_python_api)
install(DIRECTORY python_script DESTINATION .)
install(DIRECTORY cmake DESTINATION .)