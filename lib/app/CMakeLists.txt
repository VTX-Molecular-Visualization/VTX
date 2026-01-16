message("vtx_app")
cmake_minimum_required(VERSION 3.23)
project(vtx_app)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
#set(PYBIND11_PYTHON_VERSION "3.12.7")
#find_package(Python3 "3.12.7" EXACT REQUIRED COMPONENTS Interpreter Development NO_SYSTEM_ENVIRONMENT_PATH)
find_package(vtx_python_binding CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

set(_VTX_APP_CONAN " ") # So the right target name can be linked
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

install(TARGETS vtx_app FILE_SET public_headers)
install(DIRECTORY vendor/velopack/lib-static DESTINATION vendor/velopack/)
