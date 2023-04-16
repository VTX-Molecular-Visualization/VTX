message("VTX")
cmake_minimum_required(VERSION 3.16)
project(VTX)

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp)
if(MSVC)
	file(GLOB_RECURSE RESOURCES asset/windows_resources/*.rc)
endif()

# Declare executable.
add_executable(VTX ${HEADERS} ${SOURCES} ${RESOURCES})
target_include_directories(VTX PUBLIC include)

# Link libs.
add_subdirectory(lib/util)
target_link_libraries(VTX PRIVATE VTX_UTIL)
add_subdirectory(lib/ui)
target_link_libraries(VTX PRIVATE VTX_UI)

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
#win_deploy_qt(VTX)

# Copy render effect presets to build directory.
#add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory libraries ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../libraries)

# Copy Data.
#add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory internal_data ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../data)

# Readme, license and changelog.
#add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different README.md ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../README.md)
#add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different CHANGELOG.md ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../CHANGELOG.md)
#add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different license.txt ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../license.txt)
