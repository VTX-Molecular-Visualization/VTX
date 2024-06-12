message("vtx_tool")
cmake_minimum_required(VERSION 3.23)
project(vtx_tool)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_ui CONFIG REQUIRED)
find_package(vtx_tool_mdprep REQUIRED)
find_package(Eigen3 REQUIRED)

set(_VTX_TOOL_CONAN " ")
include("./cmake/library.cmake")

install(TARGETS vtx_tool FILE_SET public_headers DESTINATION include)