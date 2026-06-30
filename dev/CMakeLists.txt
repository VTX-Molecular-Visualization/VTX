set(DIR_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/../lib)
set(DIR_APPS ${CMAKE_CURRENT_SOURCE_DIR}/../app)
set(DIR_TOOLS ${CMAKE_CURRENT_SOURCE_DIR}/../tool)

include("${DIR_LIBS}/util/cmake/vtx_configure_target.cmake")
include("${DIR_LIBS}/renderer/cmake/vtx_link_cuda.cmake")

set(PYBIND11_FINDPYTHON ON)

message("vtx")
cmake_minimum_required(VERSION 3.23)

project(vtx)

find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(chemfiles CONFIG REQUIRED)
if (VTX_RENDERER)
	find_package(SDL3 CONFIG REQUIRED)
	find_package(imgui CONFIG REQUIRED)
	find_package(Ktx CONFIG REQUIRED)
endif()
find_package(stb CONFIG CONFIG REQUIRED)
find_package(tinyexr CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
if (VTX_PYTHON_BINDING)
	set (PYBIND11_PYTHON_VERSION "${CPYTHON_VERSION_MAJOR}.${CPYTHON_VERSION_MINOR}.${CPYTHON_VERSION_PATCH}")
	find_package(Python3 "${CPYTHON_VERSION_MAJOR}.${CPYTHON_VERSION_MINOR}.${CPYTHON_VERSION_PATCH}" EXACT REQUIRED COMPONENTS Interpreter Development NO_SYSTEM_ENVIRONMENT_PATH)
	find_package(pybind11 CONFIG REQUIRED)
endif()
if (VTX_UI_QT)
	find_package(Qt6 COMPONENTS Core REQUIRED)
	find_package(Qt6 COMPONENTS Gui REQUIRED)
	find_package(Qt6 COMPONENTS Widgets REQUIRED)
	find_package(Qt6 COMPONENTS Test REQUIRED)
endif()
find_package(fmt CONFIG REQUIRED)
find_package(re2 CONFIG REQUIRED)
find_package(platform_folders REQUIRED)
find_package(argparse REQUIRED)

if(LINUX)
	find_package(OpenGL REQUIRED COMPONENTS OpenGL EGL)
	find_package(X11 REQUIRED)
	find_package(wayland REQUIRED)
endif()

if (LOCAL_PDB100)
	find_package(Boost CONFIG REQUIRED) # Needed for multiprocessing on pdb database test
	find_package(LibArchive REQUIRED) # Needed for pdb100 archive reading
endif()



# Util.
include("${DIR_LIBS}/util/cmake/library.cmake")

# Core.
include("${DIR_LIBS}/core/cmake/library.cmake")

# IO.
include("${DIR_LIBS}/io/cmake/library.cmake")

# Renderer.
if (VTX_RENDERER)
	include("${DIR_LIBS}/renderer/cmake/library.cmake")
endif()

# Bench.
if (VTX_RENDERER)
	include("${DIR_APPS}/bench/cmake/library.cmake")
endif()

# Python binding.
if (VTX_PYTHON_BINDING)
	include("${DIR_LIBS}/python_binding/cmake/library.cmake")
endif()

# App.
include("${DIR_LIBS}/app/cmake/library.cmake")

# UI Qt.
if (VTX_UI_QT)
	include("${DIR_LIBS}/ui/qt/cmake/library.cmake")
endif()

# Tool example
if (VTX_UI_QT AND VTX_TOOL_EXAMPLE)
	include("${DIR_TOOLS}/example/cmake/library.cmake")
endif()

# Tool Mdprep
if (VTX_UI_QT AND VTX_TOOL_MDPREP)
	include("${DIR_TOOLS}/mdprep/cmake/library.cmake")
endif()

# Tool Topology Editor
if (VTX_UI_QT AND VTX_TOOL_TOPOLOGY_EDITOR)
	include("${DIR_TOOLS}/topology_editor/cmake/library.cmake")
endif()

# VTX.
include("${DIR_APPS}/vtx/cmake/library.cmake")


## Following projects will be local-dev exclusives. It either serves no purpose to have them built in the clould or it would be to resource-intensive.
# 

# PDB100
if (LOCAL_PDB100)
	set(PDB100_DIRECTORY_PATH "C:/Users/Valen/data/pdb/pdb100") # Needed for pdb100 related projects
	include("${CMAKE_CURRENT_LIST_DIR}/secondary_structure.cmake") # Project for secondary structure algorithm benchmark
	include("${CMAKE_CURRENT_LIST_DIR}/io_vs_pdb100.cmake") # Project for file reading benchmark
endif()
