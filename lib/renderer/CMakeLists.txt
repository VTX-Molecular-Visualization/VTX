message("VTX_RENDERER")
cmake_minimum_required(VERSION 3.16)
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

# Target compile options.
target_compile_features(VTX_RENDERER PUBLIC cxx_std_17)

# Tests.
if(NOT TARGET Catch2)
	set(GIT_URL_CATCH2 https://github.com/catchorg/Catch2.git)
	set(GIT_TAG_CATCH2 v3.3.2)

	FetchContent_Declare(Catch2 GIT_REPOSITORY ${GIT_URL_CATCH2} GIT_TAG ${GIT_TAG_CATCH2})
	FetchContent_MakeAvailable(Catch2)

	# Catch2 configuration.
	list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
	include(CTest)
	include(Catch)
endif()

# Create executable.
add_executable(VTX_RENDERER_TEST test/main.cpp)

# Link third parties.
target_link_libraries(VTX_RENDERER_TEST PRIVATE Catch2::Catch2WithMain)

# Link VTX_RENDERER.
target_link_libraries(VTX_RENDERER_TEST PRIVATE VTX_RENDERER)

# Catch2.
catch_discover_tests(VTX_RENDERER_TEST)

# Benchmark.
set(GIT_URL_GLFW https://github.com/glfw/glfw.git)
set(GIT_TAG_GLFW 3.3.8)

FetchContent_Declare(glfw GIT_REPOSITORY ${GIT_URL_GLFW} GIT_TAG ${GIT_TAG_GLFW})
FetchContent_MakeAvailable(glfw)

# Create executable.
add_executable(VTX_RENDERER_BENCH bench/main.cpp)

# Link third parties.
target_link_libraries(VTX_RENDERER_BENCH PRIVATE glfw)

# Link VTX_RENDERER.
target_link_libraries(VTX_RENDERER_BENCH PRIVATE VTX_RENDERER)