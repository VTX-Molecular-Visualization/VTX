message("VTX_UTIL")
cmake_minimum_required(VERSION 3.20)
project(VTX_UTIL)

# Git dependencies.
set(GIT_URL_GLM https://github.com/g-truc/glm.git)
set(GIT_TAG_GLM 0.9.9.8)
set(GIT_URL_SPDLOG https://github.com/gabime/spdlog.git)
set(GIT_TAG_SPDLOG v1.11.0)

# Fetch third parties.
include(FetchContent)
FetchContent_Declare(glm GIT_REPOSITORY ${GIT_URL_GLM} GIT_TAG ${GIT_TAG_GLM})
FetchContent_Declare(spdlog GIT_REPOSITORY ${GIT_URL_SPDLOG} GIT_TAG ${GIT_TAG_SPDLOG})

FetchContent_MakeAvailable(glm spdlog)

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

# Create library.
add_library(VTX_UTIL STATIC ${HEADERS} ${SOURCES})
target_include_directories(VTX_UTIL PUBLIC include)

# Link third parties.
target_link_libraries(VTX_UTIL PUBLIC glm)
target_link_libraries(VTX_UTIL PUBLIC spdlog)

# Compiler options.
target_compile_features(VTX_UTIL PUBLIC cxx_std_17)

if(CMAKE_COMPILER_IS_GNUCC)
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
	target_compile_options(VTX_UTIL PUBLIC "-Wpedantic")
	target_compile_options(VTX_UTIL PUBLIC "-Wall")
	#target_compile_options(VTX_UTIL PUBLIC "-Werror")
elseif(MSVC)
	# General.
	target_compile_options(VTX_UTIL PUBLIC "/W3")  # Warning level 3.
	target_compile_options(VTX_UTIL PUBLIC "/WX")  # Warnings as errors.
	target_compile_options(VTX_UTIL PUBLIC "/MP")  # Multicore compilation.
	target_compile_options(VTX_UTIL PUBLIC "/sdl") # Additional Security Checks.	
	target_compile_options(VTX_UTIL PUBLIC "/utf-8")
	target_compile_options(VTX_UTIL PUBLIC "/fp:fast") # Floating Point Model.
		
	# Preprocessor definitions
	#add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	#(-DNOMINMAX)

endif()

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
add_executable(VTX_UTIL_TEST test/main.cpp)

# Link third parties.
target_link_libraries(VTX_UTIL_TEST PRIVATE Catch2::Catch2WithMain)

# Link VTX_UTIL.
target_link_libraries(VTX_UTIL_TEST PRIVATE VTX_UTIL)

# Catch2.
catch_discover_tests(VTX_UTIL_TEST)