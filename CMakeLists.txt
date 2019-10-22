# Declare CMAKE required version.
cmake_minimum_required(VERSION 3.15)

# Project name.
project(VTX)

# Configuration.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)

# Create source path.
set(PATH_SRC "src")

# Get all .hpp.
file(GLOB_RECURSE HPP ${PATH_SRC}/*.hpp)
foreach(SRC IN ITEMS ${HPP})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_SRC}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Headers${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Get all .cpp.
file(GLOB_RECURSE CPP ${PATH_SRC}/*.cpp)
foreach(SRC IN ITEMS ${CPP})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_SRC}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Sources${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Find libs.
find_package(SDL2 REQUIRED)

# Declare executable with source files.
add_executable(VTX ${HPP} ${CPP})

# Link libs.
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(VTX SDL2 SDL2main)