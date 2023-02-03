message("Make VTX")

# Declare CMAKE required version.
cmake_minimum_required(VERSION 3.16)

# Project name.
project(VTX)

# Compiler options.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CONFIGS "Debug;Release") # Windows only.
set(CMAKE_CONFIGURATION_TYPES ${CONFIGS} CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)

# Add libraries.
set(PATH_LIB "lib")
set(PATH_EXTERNAL_PROJECT "external_project")

# Define libraries
set (OLD_VTX_LIB "OLD_VTX")
set (VTX_UTIL_LIB "VTX_UTIL")

set(OLD_VTX_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/${PATH_LIB}/old/)
set(VTX_UTIL_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/${PATH_LIB}/util/)
set(VTX_CORE_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/${PATH_LIB}/core/)
set(VTX_RENDERER_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/${PATH_LIB}/renderer/)
set(VTX_UI_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/${PATH_LIB}/ui/)
set(VTX_TOOL_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/${PATH_LIB}/tool/)
set(VTX_LAUNCHER_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/${PATH_LIB}/launcher/)

add_subdirectory(${CMAKE_SOURCE_DIR}/${PATH_LIB}/util/)
add_subdirectory(${CMAKE_SOURCE_DIR}/${PATH_LIB}/old/)
add_subdirectory(${CMAKE_SOURCE_DIR}/${PATH_LIB}/core/)
add_subdirectory(${CMAKE_SOURCE_DIR}/${PATH_LIB}/renderer/)
add_subdirectory(${CMAKE_SOURCE_DIR}/${PATH_LIB}/app/)
add_subdirectory(${CMAKE_SOURCE_DIR}/${PATH_LIB}/ui/)
add_subdirectory(${CMAKE_SOURCE_DIR}/${PATH_LIB}/tool/)
add_subdirectory(${CMAKE_SOURCE_DIR}/${PATH_LIB}/launcher/)

# Custom options.
option(DEFINE_PRODUCTION "Enable production mode" OFF)
if(DEFINE_PRODUCTION)
	add_definitions(-DVTX_PRODUCTION)
endif()

# Force _DEBUG preprocessor on linux.
if(CMAKE_COMPILER_IS_GNUCC)
	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		add_definitions(-D_DEBUG)
	endif()
endif()
