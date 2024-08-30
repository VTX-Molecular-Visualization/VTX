message("vtx_renderer_bench")
cmake_minimum_required(VERSION 3.23)

project(vtx_renderer_bench)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(SDL2 REQUIRED)
find_package(imgui REQUIRED)
find_package(stb CONFIG REQUIRED)
#find_package(assimp CONFIG REQUIRED)

set(_VTX_RENDERER_BENCH_CONAN " ") # So the right target name can be linked
include(cmake/library.cmake)

install(TARGETS vtx_renderer_bench)
install(DIRECTORY ${DIR_SHADERS} DESTINATION bin)

set(CPACK_PACKAGE_NAME "VTX Bench")
set(CPACK_PACKAGE_VENDOR "VTX dev team")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_EXECUTABLES "vtx_renderer_bench" "Benchmarker" )

include(CPack)