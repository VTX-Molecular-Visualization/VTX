message("Make VTX")

# Declare CMAKE required version.
cmake_minimum_required(VERSION 3.16)

# Project name.
project(VTX)

# Get CMake scripts.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
include(qt_helper)

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
set (VTX_CORE_LIB "VTX_CORE")
set (VTX_RENDERER_LIB "VTX_RENDERER")
set (VTX_APP_LIB "VTX_APP")
set (VTX_UI_LIB "VTX_UI")
set (VTX_TOOL_LIB "VTX_TOOL")
set (VTX_LAUNCHER_LIB "VTX_LAUNCHER")

set(OLD_VTX_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/${PATH_LIB}/old/)
set(VTX_UTIL_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/${PATH_LIB}/util/)
set(VTX_CORE_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/${PATH_LIB}/core/)
set(VTX_RENDERER_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/${PATH_LIB}/renderer/)
set(VTX_APP_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/${PATH_LIB}/renderer/)
set(VTX_UI_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/${PATH_LIB}/ui/)
set(VTX_TOOL_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/${PATH_LIB}/tool/)
set(VTX_LAUNCHER_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/${PATH_LIB}/launcher/)

add_subdirectory(${CMAKE_SOURCE_DIR}/${PATH_LIB}/util/)
add_subdirectory(${CMAKE_SOURCE_DIR}/${PATH_LIB}/old/)
#add_subdirectory(${CMAKE_SOURCE_DIR}/${PATH_LIB}/core/)
#add_subdirectory(${CMAKE_SOURCE_DIR}/${PATH_LIB}/renderer/)
#add_subdirectory(${CMAKE_SOURCE_DIR}/${PATH_LIB}/app/)
add_subdirectory(${CMAKE_SOURCE_DIR}/${PATH_LIB}/ui/)
add_subdirectory(${CMAKE_SOURCE_DIR}/${PATH_LIB}/tool/)
add_subdirectory(${CMAKE_SOURCE_DIR}/${PATH_LIB}/launcher/)


# Compiler options #2.
if(MSVC)
	# Set default startup project.
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${VTX_LAUNCHER_LIB})
	#set_target_properties(${VTX_LAUNCHER_LIB} PROPERTIES VS_GLOBAL_DisableFastUpToDateCheck false)
	
	# Preprocessor definitions
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DNOMINMAX)
endif()

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
