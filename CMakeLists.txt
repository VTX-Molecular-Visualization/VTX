# Declare CMAKE required version.
cmake_minimum_required(VERSION 3.15)

# Project name.
project(VTX)

# Configuration.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
#set(SDL2_DIR "lib/SDL2")

# Create source paths.
set(PATH_SRC_CORE "src/core")
set(PATH_SRC "src")
#set(PATH_SRC_IMGUI "lib/imgui")

# Get all files in /src/ folder (NOT including subfolders).
file(GLOB HPP
    "${PATH_SRC}/*.hpp"
)
file(GLOB CPP
    "${PATH_SRC}/*.cpp"
)

# Get all files in /src/core/ folder (including subfolders).
file(GLOB_RECURSE HPP_CORE
    "${PATH_SRC_CORE}/*.hpp"
)
file(GLOB_RECURSE CPP_CORE
    "${PATH_SRC_CORE}/*.cpp"
)

# Get all files in /lib/imgui-x.xx.

#file(GLOB H_IMGUI
#    "${PATH_SRC_IMGUI}/*.h"
#)
#file(GLOB CPP_IMGUI
#    "${PATH_SRC_IMGUI}/*.cpp"
#)

# SDL2.
#find_package(SDL2 REQUIRED)
#include_directories(${SDL2_DIR}/include)

# GL3W.

# Declare executable with source files.
add_executable(VTX ${HPP} ${CPP} ${HPP_CORE} ${CPP_CORE})

#find_package(gl3w CONFIG REQUIRED)
#target_link_libraries(VTX PRIVATE unofficial::gl3w::gl3w)

#find_package(SDL2 CONFIG REQUIRED)
#target_link_libraries(VTX PRIVATE SDL2::SDL2 SDL2::SDL2main)

#find_package(imgui CONFIG REQUIRED)
#target_link_libraries(VTX PRIVATE imgui::imgui)

# Link libs.
#target_link_libraries(VTX ${SDL2_LIBRARY})

# Group source files to create VS filters in solution.
source_group("Headers" FILES ${HPP})
source_group("Sources" FILES ${CPP})
source_group("Headers\\core" FILES ${HPP_CORE})
source_group("Sources\\core" FILES ${CPP_CORE})
#source_group("Headers\\lib\\imgui" FILES ${H_IMGUI})
#source_group("Sources\\lib\\imgui" FILES ${CPP_IMGUI})
