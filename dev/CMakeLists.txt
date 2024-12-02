set(DIR_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/../lib)
set(DIR_APPS ${CMAKE_CURRENT_SOURCE_DIR}/../app)
set(DIR_TOOLS ${CMAKE_CURRENT_SOURCE_DIR}/../tool)

set(PYBIND11_FINDPYTHON ON)

message("vtx")
cmake_minimum_required(VERSION 3.23)

project(vtx)

find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(chemfiles CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(stb CONFIG CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(pybind11 CONFIG REQUIRED)
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(re2 CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED) # Needed for multiprocessing on pdb database test
find_package(LibArchive REQUIRED)


# Util.
include("${DIR_LIBS}/util/cmake/library.cmake")
vtx_clear_registered_copies() # Prevent data from previous package to be copied an additional time

# Core.
include("${DIR_LIBS}/core/cmake/library.cmake")
vtx_clear_registered_copies() # Prevent data from previous package to be copied an additional time

# IO.
include("${DIR_LIBS}/io/cmake/library.cmake")
vtx_clear_registered_copies() # Prevent data from previous package to be copied an additional time

# Renderer.
include("${DIR_LIBS}/renderer/cmake/library.cmake")
vtx_clear_registered_copies() # Prevent data from previous package to be copied an additional time

# Bench.
include("${DIR_APPS}/bench/cmake/library.cmake")
vtx_clear_registered_copies() # Prevent data from previous package to be copied an additional time

# App.
include("${DIR_LIBS}/app/cmake/library.cmake")
vtx_clear_registered_copies() # Prevent data from previous package to be copied an additional time

# Python binding.
include("${DIR_LIBS}/python_binding/cmake/library.cmake")

# Python binding test.
#include("${DIR_LIBS}/python_binding/test/cmake/library.cmake")
vtx_clear_registered_copies() # Prevent data from previous package to be copied an additional time

# UI Qt.
include("${DIR_LIBS}/ui/qt/cmake/library.cmake")
vtx_clear_registered_copies() # Prevent data from previous package to be copied an additional time

# Tool example
include("${DIR_TOOLS}/example/cmake/library.cmake")
vtx_clear_registered_copies() # Prevent data from previous package to be copied an additional time

# Tool Mdprep
include("${DIR_TOOLS}/mdprep/cmake/library.cmake")
vtx_clear_registered_copies() # Prevent data from previous package to be copied an additional time

# VTX.
include("${DIR_APPS}/vtx/cmake/library.cmake")
vtx_clear_registered_copies() # Prevent data from previous package to be copied an additional time
