# Declare CMAKE required version.
cmake_minimum_required(VERSION 3.13)

# Project name.
project(VTX)

# Create paths.
set(PATH_SRC "src")
set(PATH_LIB "lib")
set(PATH_LIB_SRC "lib_src")
set(PATH_WINDOWS_RESOURCE "_windows/resource/VTX.rc")
set(CONFIGS "Debug;Release;RelWithDebInfo")

message("Downloading depdendencies...")

# Download dependencies.
set(ZIP_LIB "lib.zip")
set(ZIP_LIB_SRC "lib_src.zip")

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/${PATH_LIB})
  if(NOT EXISTS ${CMAKE_SOURCE_DIR}/${ZIP_LIB})
    file(DOWNLOAD https://www.simonguionniere.com/private/VTX/${ZIP_LIB} ${CMAKE_SOURCE_DIR}/${ZIP_LIB} SHOW_PROGRESS)
	execute_process(COMMAND ${CMAKE_COMMAND} -E tar -xf ${CMAKE_SOURCE_DIR}/${ZIP_LIB} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/${PATH_LIB})
	file(REMOVE ${CMAKE_SOURCE_DIR}/${ZIP_LIB})
  endif()
endif()

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/${PATH_LIB_SRC})
  if(NOT EXISTS ${CMAKE_SOURCE_DIR}/${ZIP_LIB_SRC})
    file(DOWNLOAD https://www.simonguionniere.com/private/VTX/${ZIP_LIB_SRC} ${CMAKE_SOURCE_DIR}/${ZIP_LIB_SRC} SHOW_PROGRESS)
	execute_process(COMMAND ${CMAKE_COMMAND} -E tar -xf ${CMAKE_SOURCE_DIR}/${ZIP_LIB_SRC} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/${PATH_LIB_SRC})
	file(REMOVE ${CMAKE_SOURCE_DIR}/${ZIP_LIB_SRC})
  endif()
endif()

message("Downloading depdendencies... DONE")
message("Copying all sources...")

# Get all .hpp.
file(GLOB_RECURSE HPP ${PATH_SRC}/*.hpp ${PATH_SRC}/*.h)
foreach(SRC IN ITEMS ${HPP})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_SRC}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Headers${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Get all C++, GLSL and CUDA sources.
file(GLOB_RECURSE CPP ${PATH_SRC}/*.cpp ${PATH_SRC}/*.vert ${PATH_SRC}/*.frag ${PATH_SRC}/*.geom ${PATH_SRC}/*.cu)
foreach(SRC IN ITEMS ${CPP})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_SRC}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Sources${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Get libs.
file(GLOB_RECURSE LIB ${PATH_LIB_SRC}/*.hpp ${PATH_LIB_SRC}/*.h ${PATH_LIB_SRC}/*.inl ${PATH_LIB_SRC}/*.c ${PATH_LIB_SRC}/*.cpp)
foreach(SRC IN ITEMS ${LIB})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_LIB_SRC}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Libs${GROUP_PATH}" FILES "${SRC}")
endforeach()

message("Copying all sources... DONE")

# Compiler options #1.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CONFIGURATION_TYPES ${CONFIGS} CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set link directories.
link_directories(${PATH_LIB}/SDL2/lib/x64)
link_directories(${PATH_LIB}/assimp/lib/x64)
link_directories(${PATH_LIB}/curl/lib/x64)
link_directories(${PATH_LIB}/chemfiles/lib/x64)

# Declare executable with source files.
add_executable(VTX ${HPP} ${CPP} ${LIB} ${PATH_WINDOWS_RESOURCE})
# Set default startup project.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT VTX)

# Link libs.
target_link_libraries(VTX PRIVATE SDL2)
target_link_libraries(VTX PRIVATE SDL2main)
target_link_libraries(VTX PRIVATE assimp-vc142-mt)
target_link_libraries(VTX PRIVATE IrrXML)
target_link_libraries(VTX PRIVATE zlib)
target_link_libraries(VTX PRIVATE zlibstatic)
target_link_libraries(VTX PRIVATE libcurl)
target_link_libraries(VTX PRIVATE chemfiles)

if(WIN32)
	# Add specific libs.
	target_link_libraries(VTX PRIVATE ws2_32)
  
	# Copy DLL to output.	
	# Debug
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/SDL2/bin/x64/Debug/SDL2.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/SDL2.dll)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/assimp/bin/x64/Debug/assimp-vc142-mtd.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/assimp-vc142-mtd.dll)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/assimp/bin/x64/Debug/zlibd.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/zlibd1.dll)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/curl/bin/x64/Debug/libcurl-d.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/libcurl-d.dll)
	# Release
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/SDL2/bin/x64/Release/SDL2.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/SDL2.dll)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/assimp/bin/x64/Release/assimp-vc142-mt.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/assimp-vc142-mt.dll)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/assimp/bin/x64/Release/zlib.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/zlib1.dll)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/curl/bin/x64/Release/libcurl.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/libcurl.dll)
	# RelWithDebInfo
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/SDL2/bin/x64/RelWithDebInfo/SDL2.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/SDL2.dll)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/assimp/bin/x64/RelWithDebInfo/assimp-vc142-mt.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/assimp-vc142-mt.dll)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/assimp/bin/x64/RelWithDebInfo/zlib.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/zlib1.dll)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/curl/bin/x64/RelWithDebInfo/libcurl.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/libcurl.dll)
	
	# Copy shaders to build directoy.
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/${PATH_SRC}/shader ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../shaders)
	
	# Copy config files.
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/config ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/..)

endif()

# Includes directories.
target_include_directories(VTX PRIVATE ${PATH_SRC})
target_include_directories(VTX PRIVATE ${PATH_LIB})
target_include_directories(VTX PRIVATE ${PATH_LIB_SRC})
target_include_directories(VTX PRIVATE ${PATH_LIB_SRC}/GL)
target_include_directories(VTX PRIVATE ${PATH_LIB}/msgpack)
target_include_directories(VTX PRIVATE ${PATH_LIB}/SDL2/include)
target_include_directories(VTX PRIVATE ${PATH_LIB}/assimp/include)
target_include_directories(VTX PRIVATE ${PATH_LIB}/curl/include)
target_include_directories(VTX PRIVATE ${PATH_LIB}/chemfiles/include)

# Compiler options #2.
if(CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(VTX PRIVATE "-Wall -Wextra")
elseif(MSVC)
	#General.
    target_compile_options(VTX PRIVATE "/W3")  # Warning level 3.
	target_compile_options(VTX PRIVATE "/WX")  # Warnings as errors.
	target_compile_options(VTX PRIVATE "/MP")  # Multicore compilation.
	target_compile_options(VTX PRIVATE "/sdl") # SDL check.
	#Optimizaion.
	target_compile_options(VTX PRIVATE "/Ot")  # Favor speed.
	#Code Generation.
	target_compile_options(VTX PRIVATE "/fp:fast") # Floating Point Model.
	#Preprocessor definitions
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DNOMINMAX)
endif()

if(WIN32)
	add_definitions(-DCHEMFILES_WINDOWS)	
endif()

# TODO: find cuda and optix.
if(WIN32)
	add_definitions(-DCUDA_DEFINED)
	add_definitions(-DOPTIX_DEFINED)
	target_include_directories(VTX PRIVATE "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.2/include")
	target_include_directories(VTX PRIVATE "C:/ProgramData/NVIDIA Corporation/OptiX SDK 7.0.0/include")
	target_link_libraries(VTX PRIVATE "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.2/lib/x64/cudart_static.lib")
	target_link_libraries(VTX PRIVATE "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.2/lib/x64/cuda.lib")
endif()