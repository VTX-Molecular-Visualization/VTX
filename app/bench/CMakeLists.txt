message("vtx_renderer_bench")
cmake_minimum_required(VERSION 3.23)

project(vtx_renderer_bench)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(imgui REQUIRED)

set(_VTX_RENDERER_BENCH_CONAN " ")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

set_target_properties(vtx_renderer_bench PROPERTIES OUTPUT_NAME bench)

set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include(InstallRequiredSystemLibraries)

install(TARGETS vtx_renderer_bench DESTINATION .)
install(DIRECTORY ${DIR_SHADERS} DESTINATION .)
install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION .)

set(CPACK_PACKAGE_NAME "VTX Benchmarker")
set(CPACK_PACKAGE_VENDOR "VTX dev team")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_EXECUTABLES "bench" "Benchmarker" )
set(CPACK_NSIS_MENU_LINKS "bench.exe" "Benchmarker")

#cmake_path(CONVERT $ENV{APPDATA} TO_CMAKE_PATH_LIST PATH_APP_DATA NORMALIZE)
#set(CPACK_NSIS_INSTALL_ROOT "${PATH_APP_DATA}")
set(CPACK_NSIS_INSTALL_ROOT "$LOCALAPPDATA")

include(CPack)