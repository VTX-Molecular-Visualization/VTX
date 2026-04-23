message("vtx_tool_topology_editor")
cmake_minimum_required(VERSION 3.23)
project(vtx_tool_topology_editor)

# Internal lib packages.
if (VTX_PYTHON_BINDING)
	set(PYBIND11_PYTHON_VERSION "${CPYTHON_VERSION_MAJOR}.${CPYTHON_VERSION_MINOR}.${CPYTHON_VERSION_PATCH}")
	find_package(Python3 "${CPYTHON_VERSION_MAJOR}.${CPYTHON_VERSION_MINOR}.${CPYTHON_VERSION_PATCH}" EXACT REQUIRED COMPONENTS Interpreter Development NO_SYSTEM_ENVIRONMENT_PATH)
endif()
find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_ui_qt CONFIG REQUIRED)

# External dependencies.
find_package(Catch2 CONFIG REQUIRED)

# Workaround.
set(_VTX_TOOL_TOPOLOGY_EDITOR_CONAN " ")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

# Install target and headers.
install(TARGETS vtx_tool_topology_editor FILE_SET public_headers)
if (VTX_UI_QT)
	vtx_qt_install_runtime(vtx_tool_topology_editor)
endif()
