message("VTX_UTIL")

# Git dependencies.
set(GIT_URL_GLM https://github.com/g-truc/glm.git)
set(GIT_TAG_GLM 0.9.9.8)
set(GIT_URL_SPDLOG https://github.com/gabime/spdlog.git)
set(GIT_TAG_SPDLOG v1.11.0)
set(GIT_URL_CATCH2 https://github.com/catchorg/Catch2.git)
set(GIT_TAG_CATCH2 v3.3.2)
  
cmake_minimum_required(VERSION 3.16)
project(VTX_UTIL)

# Fetch third parties.
include(FetchContent)
FetchContent_Declare(glm GIT_REPOSITORY ${GIT_URL_GLM} GIT_TAG ${GIT_TAG_GLM})
FetchContent_MakeAvailable(glm)
FetchContent_Declare(spdlog GIT_REPOSITORY ${GIT_URL_SPDLOG} GIT_TAG ${GIT_TAG_SPDLOG})
FetchContent_MakeAvailable(spdlog)

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp)

# Create library.
add_library(VTX_UTIL STATIC ${HEADERS} ${SOURCES})
target_include_directories(VTX_UTIL PUBLIC include)

# Link third parties.
target_link_libraries(VTX_UTIL PUBLIC glm)
target_link_libraries(VTX_UTIL PRIVATE spdlog)

# Target compile options.
target_compile_features(VTX_UTIL PUBLIC cxx_std_17)