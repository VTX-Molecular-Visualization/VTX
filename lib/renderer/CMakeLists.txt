message("vtx_renderer")
cmake_minimum_required(VERSION 3.23)
project(vtx_renderer)

find_package(vtx_util CONFIG REQUIRED)

file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)
file(GLOB_RECURSE VENDORS vendor/*)

add_library(vtx_renderer STATIC ${HEADERS} ${SOURCES} ${VENDORS})
target_include_directories(vtx_renderer PUBLIC include)
# TODO: PRIVATE
target_include_directories(vtx_renderer PUBLIC vendor)

target_link_libraries(vtx_renderer PRIVATE vtx_util::vtx_util)

# Copy shaders.
add_custom_target(vtx_renderer_COPY_SHADERS ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/renderer/context/gl/shaders $<TARGET_FILE_DIR:vtx_renderer>/shaders)
add_dependencies(vtx_renderer vtx_renderer_COPY_SHADERS)

install(TARGETS vtx_renderer)