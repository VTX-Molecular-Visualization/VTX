message("VTX_APP")
cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
project(VTX_APP)

# Git dependencies.
set(GIT_URL_CHEMFILES https://github.com/VTX-Molecular-Visualization/chemfiles.git)
set(GIT_TAG_CHEMFILES d060165971aaf4f62358ff0aed68837f2257ca0c)
set(GIT_URL_ENTT https://github.com/skypjack/entt.git)
set(GIT_TAG_ENTT fef921132cae7588213d0f9bcd2fb9c8ffd8b7fc)
set(GIT_URL_JSON https://github.com/nlohmann/json.git)
set(GIT_TAG_JSON v3.11.2)
set(GIT_URL_MAGIC_ENUM https://github.com/Neargye/magic_enum.git)
set(GIT_TAG_MAGIC_ENUM v0.8.2)

# Fetch third parties.
include(FetchContent)
FetchContent_Declare(chemfiles GIT_REPOSITORY ${GIT_URL_CHEMFILES} GIT_TAG ${GIT_TAG_CHEMFILES})
FetchContent_Declare(EnTT GIT_REPOSITORY ${GIT_URL_ENTT} GIT_TAG ${GIT_TAG_ENTT})
FetchContent_Declare(nlohmann_json GIT_REPOSITORY ${GIT_URL_JSON} GIT_TAG ${GIT_TAG_JSON})
FetchContent_Declare(magic_enum GIT_REPOSITORY ${GIT_URL_MAGIC_ENUM} GIT_TAG ${GIT_TAG_MAGIC_ENUM})

FetchContent_MakeAvailable(chemfiles EnTT nlohmann_json magic_enum)

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

# Create library.
add_library(VTX_APP STATIC ${HEADERS} ${SOURCES})
target_include_directories(VTX_APP PUBLIC include)

# Link VTX_UTIL.
if(NOT TARGET VTX_UTIL)
	add_subdirectory(../util lib/util)
endif()
target_link_libraries(VTX_APP PRIVATE VTX_UTIL)

# Link VTX_CORE.
add_subdirectory(../core lib/core)
target_link_libraries(VTX_APP PUBLIC VTX_CORE)

# Link VTX_RENDERER.
add_subdirectory(../renderer lib/renderer)
target_link_libraries(VTX_APP PRIVATE VTX_RENDERER)


# Link third parties.
target_link_libraries(VTX_APP PUBLIC chemfiles)
target_link_libraries(VTX_APP PUBLIC EnTT::EnTT)
target_link_libraries(VTX_APP PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(VTX_APP PUBLIC magic_enum)



# Test.
# Create executable.
add_executable(VTX_APP_TEST test/main.cpp)

# Link third parties.
target_link_libraries(VTX_APP_TEST PRIVATE Catch2::Catch2WithMain)
target_link_libraries(VTX_APP_TEST PRIVATE glfw)

# Link VTX_APP.
target_link_libraries(VTX_APP_TEST PRIVATE VTX_UTIL)
target_link_libraries(VTX_APP_TEST PRIVATE VTX_APP)
target_link_libraries(VTX_APP_TEST PRIVATE VTX_RENDERER)

# Catch2.
include(CTest)
catch_discover_tests(VTX_APP_TEST)