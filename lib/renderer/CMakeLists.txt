message("vtx_renderer")
cmake_minimum_required(VERSION 3.23)
project(vtx_renderer)

if(LINUX)
	find_package(OpenGL REQUIRED COMPONENTS OpenGL EGL)
endif()
find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

set(_VTX_RENDERER_CONAN " ") # So the right target name can be linked
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

install(TARGETS vtx_renderer FILE_SET public_headers DESTINATION include)
vtx_install_source_directory("shaders")
vtx_install_build_modules()
