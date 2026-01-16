message("vtx_python_binding")
cmake_minimum_required(VERSION 3.23)

project(vtx_python_binding)


set (PYBIND11_PYTHON_VERSION "${CPYTHON_VERSION_MAJOR}.${CPYTHON_VERSION_MINOR}.${CPYTHON_VERSION_PATCH}")
find_package(Python3 "${CPYTHON_VERSION_MAJOR}.${CPYTHON_VERSION_MINOR}.${CPYTHON_VERSION_PATCH}" EXACT REQUIRED COMPONENTS Interpreter Development NO_SYSTEM_ENVIRONMENT_PATH)
message("VTX - Found Python3: ${Python3_EXECUTABLE}")
message("VTX - Python3 version: ${Python3_VERSION}")
message("VTX - Python3 library: ${Python3_LIBRARIES}")
message("VTX - Python3 include: ${Python3_INCLUDE_DIRS}")

# Disable pybind11's own Python finding since we already have it from Conan
set(PYBIND11_FINDPYTHON OFF)
set(PYBIND11_PYTHON_VERSION ${Python3_VERSION})

# Set pybind11 to use our already-found Python
set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
set(PYTHON_INCLUDE_DIR ${Python3_INCLUDE_DIRS})
set(PYTHON_LIBRARY ${Python3_LIBRARIES})

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
install(DIRECTORY python_script DESTINATION .)
install(DIRECTORY cmake DESTINATION .)