message("vtx_tool_mdprep")
cmake_minimum_required(VERSION 3.23)

project(vtx_tool_mdprep)

set (PYBIND11_PYTHON_VERSION "${CPYTHON_VERSION_MAJOR}.${CPYTHON_VERSION_MINOR}.${CPYTHON_VERSION_PATCH}")
find_package(Python3 "${CPYTHON_VERSION_MAJOR}.${CPYTHON_VERSION_MINOR}.${CPYTHON_VERSION_PATCH}" EXACT REQUIRED COMPONENTS Interpreter Development NO_SYSTEM_ENVIRONMENT_PATH)
find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_ui_qt CONFIG REQUIRED)
find_package(re2 CONFIG REQUIRED)
find_package(Catch2 REQUIRED)

set(_VTX_MDPREP_CONAN " ") # So the right target name can be linked
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

install(TARGETS vtx_tool_mdprep FILE_SET public_headers)
install(DIRECTORY cmake DESTINATION .)
install(DIRECTORY data DESTINATION .)
