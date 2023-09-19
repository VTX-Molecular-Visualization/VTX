message("VTX_APP")
cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
project(VTX_APP)

# Git dependencies.
set(GIT_URL_ENTT https://github.com/skypjack/entt.git)
set(GIT_TAG_ENTT fef921132cae7588213d0f9bcd2fb9c8ffd8b7fc)

# Fetch third parties.
include(FetchContent)
FetchContent_Declare(EnTT GIT_REPOSITORY ${GIT_URL_ENTT} GIT_TAG ${GIT_TAG_ENTT})

FetchContent_MakeAvailable(EnTT)

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

# Create library.
add_library(VTX_APP STATIC ${HEADERS} ${SOURCES})
target_include_directories(VTX_APP PUBLIC include)

# Link VTX modules.
if(NOT TARGET VTX_UTIL)
	add_subdirectory(../util lib/util)
endif()
if(NOT TARGET VTX_CORE)
	add_subdirectory(../core lib/core)
endif()
if(NOT TARGET VTX_RENDERER)
	add_subdirectory(../renderer lib/renderer)
endif()
if(NOT TARGET VTX_IO)
	add_subdirectory(../io lib/io)
endif()
target_link_libraries(VTX_APP PRIVATE VTX_UTIL)
target_link_libraries(VTX_APP PRIVATE VTX_RENDERER)
target_link_libraries(VTX_APP PRIVATE VTX_CORE)
target_link_libraries(VTX_APP PRIVATE VTX_IO)

# Link third parties.
target_link_libraries(VTX_APP PRIVATE EnTT::EnTT)

## Test.
file(GLOB_RECURSE TEST_SOURCES test/*.hpp test/*.cpp)
add_executable(VTX_APP_TEST ${TEST_SOURCES})

# Link third parties.
target_link_libraries(VTX_APP_TEST PRIVATE Catch2::Catch2WithMain)
target_link_libraries(VTX_APP_TEST PRIVATE EnTT::EnTT)

# Link VTX_APP.
target_link_libraries(VTX_APP_TEST PRIVATE VTX_UTIL)
target_link_libraries(VTX_APP_TEST PRIVATE VTX_CORE)
target_link_libraries(VTX_APP_TEST PRIVATE VTX_IO)
target_link_libraries(VTX_APP_TEST PRIVATE VTX_RENDERER)
target_link_libraries(VTX_APP_TEST PRIVATE VTX_APP)

# Catch2.
include(CTest)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)	
catch_discover_tests(VTX_APP_TEST)
endif()

# Copy data.
add_custom_target(VTX_APP_TEST_COPY_DATA ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/test/data $<TARGET_FILE_DIR:VTX_APP_TEST>/data)
add_dependencies(VTX_APP_TEST VTX_APP_TEST_COPY_DATA)