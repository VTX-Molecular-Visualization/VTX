message("VTX_UTIL_TEST")
cmake_minimum_required(VERSION 3.23)

project(VTX_UTIL_TEST)

# Find packages.
find_package(vtx_util CONFIG REQUIRED)
find_package(Catch2 REQUIRED)

# Catch2 configuration.
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

# Create executable.
add_executable(VTX_UTIL_TEST src/main.cpp)
#configureTarget(VTX_UTIL_TEST)

# Link third parties.
target_link_libraries(VTX_UTIL_TEST PRIVATE Catch2::Catch2WithMain)
target_link_libraries(VTX_UTIL_TEST PRIVATE vtx_util::vtx_util)

# Catch2.
include(CTest)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	catch_discover_tests(VTX_UTIL_TEST)
endif()