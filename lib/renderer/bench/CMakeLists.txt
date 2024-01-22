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
find_package(assimp CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES src/*)
file(GLOB_RECURSE VENDORS vendor/*)

add_executable(vtx_renderer_bench ${SOURCES} ${VENDORS})
configure_target(vtx_renderer_bench)

target_include_directories(vtx_renderer_bench PRIVATE vendor)

target_link_libraries(vtx_renderer_bench PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_renderer_bench PRIVATE vtx_renderer::vtx_renderer)
target_link_libraries(vtx_renderer_bench PRIVATE vtx_core::vtx_core)
target_link_libraries(vtx_renderer_bench PRIVATE vtx_io::vtx_io)
target_link_libraries(vtx_renderer_bench PRIVATE SDL2::SDL2-static)
target_link_libraries(vtx_renderer_bench PRIVATE SDL2::SDL2main)
target_link_libraries(vtx_renderer_bench PRIVATE imgui::imgui)
target_link_libraries(vtx_renderer_bench PRIVATE stb::stb)
target_link_libraries(vtx_renderer_bench PRIVATE assimp::assimp)

add_custom_target(
	vtx_renderer_bench_copy_shader
	ALL COMMAND
	${CMAKE_COMMAND} -E copy_directory	
	${DIR_SHADERS}
	$<TARGET_FILE_DIR:vtx_renderer_bench>/shaders)
add_dependencies(vtx_renderer_bench vtx_renderer_bench_copy_shader)

install(TARGETS vtx_renderer_bench)
install(DIRECTORY ${DIR_SHADERS} DESTINATION bin)
