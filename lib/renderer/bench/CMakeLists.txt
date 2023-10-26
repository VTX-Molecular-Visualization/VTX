message("vtx_renderer_bench")
cmake_minimum_required(VERSION 3.23)
project(vtx_renderer_bench)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(SDL2 REQUIRED)

file(GLOB_RECURSE SOURCES src/*)
add_executable(vtx_renderer_bench ${SOURCES})

target_include_directories(vtx_renderer_bench PRIVATE src/imgui)
target_include_directories(vtx_renderer_bench PRIVATE src/stb)
target_include_directories(vtx_renderer_bench PRIVATE ${SDL2_INCLUDE_DIRS})


target_link_libraries(vtx_renderer_bench PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_renderer_bench PRIVATE vtx_renderer::vtx_renderer)
target_link_libraries(vtx_renderer_bench PRIVATE vtx_core::vtx_core)
target_link_libraries(vtx_renderer_bench PRIVATE vtx_io::vtx_io)
target_link_libraries(vtx_renderer_bench PRIVATE SDL2::SDL2-static)
target_link_libraries(vtx_renderer_bench PRIVATE SDL2::SDL2main)

# Copy assets.
#add_custom_target(VTX_RENDERER_COPY_ASSETS ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:vtx_renderer_bench>/assets)
#add_dependencies(vtx_renderer_bench VTX_RENDERER_COPY_ASSETS)

# Install.
# TODO: set target name as parameter?
#set(INSTALL_DIR DESTINATION VTXRendererBench)
#install(DIRECTORY DESTINATION ${INSTALL_DIR})
#install(TARGETS vtx_renderer_bench RUNTIME DESTINATION ${INSTALL_DIR})
#install(DIRECTORY $<TARGET_FILE_DIR:vtx_renderer_bench>/shaders DESTINATION ${INSTALL_DIR})