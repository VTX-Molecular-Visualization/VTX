message("VTX")
cmake_minimum_required(VERSION 3.21)
project(VTX)

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)
if(MSVC)
	file(GLOB_RECURSE RESOURCES asset/windows_resources/*.rc)
endif()

# Declare executable.
add_executable(VTX ${HEADERS} ${SOURCES} ${RESOURCES})
target_include_directories(VTX PUBLIC include)

# Link VTX modules.
if(NOT TARGET VTX_UTIL)
	add_subdirectory(lib/util)
endif()
if(NOT TARGET VTX_UI)
	add_subdirectory(lib/ui)
endif()
if(NOT TARGET VTX_TOOL)
	add_subdirectory(lib/tool)
endif()
target_link_libraries(VTX PRIVATE VTX_UTIL)
target_link_libraries(VTX PRIVATE VTX_UI)
target_link_libraries(VTX PRIVATE VTX_TOOL)

# Copy dlls in .exe directory
if(MSVC)
 	add_custom_command(TARGET VTX POST_BUILD
 	  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:VTX> $<TARGET_FILE_DIR:VTX>
 	  COMMAND_EXPAND_LISTS
 	)
endif()

# Copy Qt plugin DLLs into target dir
win_deploy_qt(VTX)

# TODO: move in corresponding lib.
if(MSVC)
	# Preprocessor definitions
	#add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	#(-DNOMINMAX)
endif()

# Custom options.
option(DEFINE_PRODUCTION "Enable production mode" OFF)
if(DEFINE_PRODUCTION)
	add_definitions(-DVTX_PRODUCTION)
endif()

# TODO: better solution?
# Force _DEBUG preprocessor on linux.
if(CMAKE_COMPILER_IS_GNUCC)
	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		add_definitions(-D_DEBUG)
	endif()
endif()

# TODO: move in corresponding lib.
# Get CMake helper functions.
# list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
# include(qt_helper)
# win_deploy_qt(VTX)

# Copy render effect presets to build directory.
#add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory libraries ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../libraries)

# Copy Data.
#add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory internal_data ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../data)

# Readme, license and changelog.
#add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different README.md ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../README.md)
#add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different CHANGELOG.md ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../CHANGELOG.md)
#add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different license.txt ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../license.txt)
