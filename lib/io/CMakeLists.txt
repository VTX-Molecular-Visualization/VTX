message("VTX_IO")
cmake_minimum_required(VERSION 3.20)
include(../util/cmake/configure-target.cmake)
project(VTX_IO)

# Git dependencies
set(GIT_URL_CHEMFILES https://github.com/VTX-Molecular-Visualization/chemfiles.git)
set(GIT_TAG_CHEMFILES master)

# Fetch third parties.
include(FetchContent)
FetchContent_Declare(chemfiles GIT_REPOSITORY ${GIT_URL_CHEMFILES} GIT_TAG ${GIT_TAG_CHEMFILES})

FetchContent_MakeAvailable(chemfiles)

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

# Create library.
add_library(VTX_IO STATIC ${HEADERS} ${SOURCES})
configureTarget(VTX_IO)
target_include_directories(VTX_IO PUBLIC include)

# Link VTX_UTIL.
if(NOT TARGET VTX_UTIL)
	add_subdirectory(../util lib/util)
endif()
target_link_libraries(VTX_IO PRIVATE VTX_UTIL)

# Link VTX_CORE.
if(NOT TARGET VTX_CORE)
	add_subdirectory(../core lib/core)
endif()
target_link_libraries(VTX_IO PRIVATE VTX_CORE)

# Link third parties.
target_link_libraries(VTX_IO PRIVATE chemfiles)

# Test.
# Create executable.
file(GLOB_RECURSE TEST_FILES test/*)
add_executable(VTX_IO_TEST ${TEST_FILES})
configureTarget(VTX_IO_TEST)

# Link Catch2.
target_link_libraries(VTX_IO_TEST PRIVATE Catch2::Catch2WithMain)

# Link VTX Modules.
target_link_libraries(VTX_IO_TEST PRIVATE VTX_UTIL)
target_link_libraries(VTX_IO_TEST PRIVATE VTX_CORE)
target_link_libraries(VTX_IO_TEST PRIVATE VTX_IO)

# Link third parties.
target_link_libraries(VTX_IO_TEST PRIVATE chemfiles)

# Catch2.
include(CTest)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	catch_discover_tests(VTX_IO_TEST)
endif()

# Copy data.
add_custom_target(VTX_IO_TEST_COPY_DATA ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/test/data $<TARGET_FILE_DIR:VTX_IO_TEST>/data)
add_dependencies(VTX_IO_TEST VTX_IO_TEST_COPY_DATA)