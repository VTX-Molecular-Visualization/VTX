message("vtx_renderer_bench")
cmake_minimum_required(VERSION 3.23)

project(vtx_renderer_bench)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(imgui REQUIRED)
#find_package(assimp CONFIG REQUIRED)

set(_VTX_RENDERER_BENCH_CONAN " ") # So the right target name can be linked
include(cmake/library.cmake)

set_target_properties(vtx_renderer_bench PROPERTIES OUTPUT_NAME bench)

install(TARGETS vtx_renderer_bench DESTINATION .)
install(DIRECTORY ${DIR_SHADERS} DESTINATION .)

set(CPACK_PACKAGE_NAME "VTX Benchmarker")
set(CPACK_PACKAGE_VENDOR "VTX dev team")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_EXECUTABLES "bench" "Benchmarker" )

include(CPack)