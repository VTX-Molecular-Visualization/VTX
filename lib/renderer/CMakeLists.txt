message("VTX_RENDERER")
cmake_minimum_required(VERSION 3.21)
project(VTX_RENDERER)

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

# Create library.
add_library(VTX_RENDERER STATIC ${HEADERS} ${SOURCES})
target_include_directories(VTX_RENDERER PUBLIC include)

# Link VTX_UTIL.
if(NOT TARGET VTX_UTIL)
	add_subdirectory(../util lib/util)
endif()
target_link_libraries(VTX_RENDERER PUBLIC VTX_UTIL)

# Copy shaders.
add_custom_target(VTX_RENDERER_COPY_SHADERS ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/renderer/gl/shaders $<TARGET_FILE_DIR:VTX_RENDERER>/shaders)
add_dependencies(VTX_RENDERER VTX_RENDERER_COPY_SHADERS)

# Benchmark.
set(GIT_URL_SDL https://github.com/libsdl-org/SDL.git)
set(GIT_TAG_SDL release-2.26.5)

FetchContent_Declare(SDL2 GIT_REPOSITORY ${GIT_URL_SDL} GIT_TAG ${GIT_TAG_SDL})
FetchContent_MakeAvailable(SDL2)

file(GLOB_RECURSE BENCH_FILES bench/*)
add_executable(VTX_RENDERER_BENCH ${BENCH_FILES})
target_include_directories(VTX_RENDERER_BENCH PUBLIC bench/imgui)
target_include_directories(VTX_RENDERER_BENCH PUBLIC bench/stb)
target_include_directories(VTX_RENDERER_BENCH PUBLIC ${SDL2_SOURCE_DIR}/include)

# Link third parties.
target_link_libraries(VTX_RENDERER_BENCH PRIVATE SDL2::SDL2)
target_link_libraries(VTX_RENDERER_BENCH PRIVATE SDL2::SDL2main)

# Link VTX_RENDERER.
target_link_libraries(VTX_RENDERER_BENCH PRIVATE VTX_RENDERER)

# Link VTX_CORE.
if(NOT TARGET VTX_CORE)
	add_subdirectory(../core lib/core)
endif()
target_link_libraries(VTX_RENDERER_BENCH PUBLIC VTX_CORE)

# Copy DLLs.
add_custom_command(TARGET VTX_RENDERER_BENCH POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:VTX_RENDERER_BENCH> $<TARGET_RUNTIME_DLLS:VTX_RENDERER_BENCH>
  COMMAND_EXPAND_LISTS
)

# Copy assets.
add_custom_target(VTX_RENDERER_COPY_ASSETS ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:VTX_RENDERER_BENCH>/assets)
add_dependencies(VTX_RENDERER_BENCH VTX_RENDERER_COPY_ASSETS)