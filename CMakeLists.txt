# Declare CMAKE required version.
cmake_minimum_required(VERSION 3.15)

# Project name.
project(VTX)

# Create source path.
set(PATH_SRC "src")

# Get all .hpp.
file(GLOB_RECURSE HPP ${PATH_SRC}/*.hpp ${PATH_SRC}/*.h)
foreach(SRC IN ITEMS ${HPP})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_SRC}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Headers${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Get all .cpp.
file(GLOB_RECURSE CPP ${PATH_SRC}/*.cpp ${PATH_SRC}/*.vert ${PATH_SRC}/*.frag)
foreach(SRC IN ITEMS ${CPP})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_SRC}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Sources${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Find libs.
find_package(SDL2 CONFIG REQUIRED)
find_package(gl3w CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(msgpack CONFIG REQUIRED)

# Compiler options #1.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)


# Declare executable with source files.
add_executable(VTX ${HPP} ${CPP})

# Link libs.
target_link_libraries(VTX PRIVATE SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(VTX PRIVATE unofficial::gl3w::gl3w)
target_link_libraries(VTX PRIVATE glm)
target_link_libraries(VTX PRIVATE msgpackc msgpackc-cxx)
if(WIN32)
  target_link_libraries(VTX PRIVATE ws2_32)
endif()

# Compiler options #2.
if(CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(VTX PRIVATE "-Wall -Wextra")
endif()
if(MSVC)
    target_compile_options(VTX PRIVATE "/W3")
	target_compile_options(VTX PRIVATE "/MP")
endif()