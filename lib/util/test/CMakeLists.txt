message("VTX_UTIL_TEST")

# Git dependencies.
set(GIT_URL_CATCH2 https://github.com/catchorg/Catch2.git)
set(GIT_TAG_CATCH2 v3.3.2)
  
cmake_minimum_required(VERSION 3.16)
project(VTX_UTIL_TEST)

# Fetch third parties.
include(FetchContent)
FetchContent_Declare(Catch2 GIT_REPOSITORY ${GIT_URL_CATCH2} GIT_TAG ${GIT_TAG_CATCH2})
FetchContent_MakeAvailable(Catch2)

# Get VTX_UTIL
add_subdirectory(.. lib)

# Create executable.
add_executable(VTX_UTIL_TEST main.cpp)

# Link third parties.
target_link_libraries(VTX_UTIL_TEST PRIVATE Catch2::Catch2WithMain)

# Link VTX_UTIL.
target_link_libraries(VTX_UTIL_TEST PRIVATE VTX_UTIL)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(VTX_UTIL_TEST)

