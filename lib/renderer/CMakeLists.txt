message("VTX_RENDERER")
cmake_minimum_required(VERSION 3.20)
project(VTX_RENDERER)

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

# Create library.
add_library(VTX_RENDERER STATIC ${HEADERS} ${SOURCES})
target_include_directories(VTX_RENDERER PUBLIC include)

# Link VTX_UTIL.
if(NOT TARGET VTX_UTIL)
	add_subdirectory(../util lib)
endif()
target_link_libraries(VTX_RENDERER PUBLIC VTX_UTIL)

# Copy shaders.
add_custom_target(VTX_RENDERER_COPY_SHADERS ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/renderer/gl/shaders $<TARGET_FILE_DIR:VTX_RENDERER>/shaders)
add_dependencies(VTX_RENDERER VTX_RENDERER_COPY_SHADERS)

# Tests.
set(GIT_URL_GLFW https://github.com/glfw/glfw.git)
set(GIT_TAG_GLFW 3.3.8)

FetchContent_Declare(glfw GIT_REPOSITORY ${GIT_URL_GLFW} GIT_TAG ${GIT_TAG_GLFW})
FetchContent_MakeAvailable(glfw)

# Create executable.
add_executable(VTX_RENDERER_TEST test/main.cpp)

# Link third parties.
target_link_libraries(VTX_RENDERER_TEST PRIVATE Catch2::Catch2WithMain)
target_link_libraries(VTX_RENDERER_TEST PRIVATE glfw)

# Link VTX_RENDERER.
target_link_libraries(VTX_RENDERER_TEST PRIVATE VTX_RENDERER)

# Catch2.
catch_discover_tests(VTX_RENDERER_TEST)

# Create executable.
add_executable(VTX_RENDERER_BENCH bench/main.cpp)

# Link third parties.
target_link_libraries(VTX_RENDERER_BENCH PRIVATE glfw)

# Link VTX_RENDERER.
target_link_libraries(VTX_RENDERER_BENCH PRIVATE VTX_RENDERER)