message("vtx_tool_mdprep")
cmake_minimum_required(VERSION 3.23)

project(vtx_tool_mdprep)

if (NOT DEFINED VTX_RENDERER)
	set(VTX_RENDERER 1)
endif()
if (NOT DEFINED VTX_PYTHON_BINDING)
	set(VTX_PYTHON_BINDING 1)
endif()

if (VTX_PYTHON_BINDING)
	set(PYBIND11_PYTHON_VERSION "${CPYTHON_VERSION_MAJOR}.${CPYTHON_VERSION_MINOR}.${CPYTHON_VERSION_PATCH}")
	find_package(Python3 "${CPYTHON_VERSION_MAJOR}.${CPYTHON_VERSION_MINOR}.${CPYTHON_VERSION_PATCH}" EXACT REQUIRED COMPONENTS Interpreter Development NO_SYSTEM_ENVIRONMENT_PATH)
endif()
find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_ui_qt CONFIG REQUIRED)
find_package(re2 CONFIG REQUIRED)
find_package(Catch2 REQUIRED)

set(_VTX_MDPREP_CONAN " ") # So the right target name can be linked
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

install(TARGETS vtx_tool_mdprep FILE_SET public_headers)
vtx_install_build_modules()
vtx_qt_install_runtime(vtx_tool_mdprep)
vtx_install_source_directory("data")
