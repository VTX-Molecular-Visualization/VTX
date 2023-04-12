message("Project library: VTX_UTIL")

set(GIT_URL_GLM https://github.com/g-truc/glm.git)
set(GIT_TAG_GLM 0.9.9.8)
set(GIT_URL_SPDLOG https://github.com/gabime/spdlog.git)
set(GIT_TAG_SPDLOG v1.11.0)
set(GIT_URL_CATCH2 https://github.com/catchorg/Catch2.git)
set(GIT_TAG_CATCH2 v3.3.2)
  
cmake_minimum_required(VERSION 3.16)
project(VTX_UTIL)

include(ExternalProject)
ExternalProject_Add(
    glm
    PREFIX external_project/g-truc
    GIT_REPOSITORY ${GIT_URL_GLM}
	GIT_TAG ${GIT_TAG_GLM}
	CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    )
ExternalProject_Get_Property(glm SOURCE_DIR)
set(GLM_INCLUDE_DIR ${SOURCE_DIR})

ExternalProject_Add(
    spdlog
    PREFIX external_project/gabime
    GIT_REPOSITORY ${GIT_URL_SPDLOG}
	GIT_TAG ${GIT_TAG_SPDLOG}
	CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    )
ExternalProject_Get_Property(spdlog SOURCE_DIR)
set(SPDLOG_INCLUDE_DIR ${SOURCE_DIR})

file(GLOB_RECURSE SOURCES src/*.cpp)
add_library(VTX_UTIL SHARED ${SOURCES})

target_include_directories(VTX_UTIL PUBLIC include)
target_include_directories(VTX_UTIL PRIVATE include/util)
target_include_directories(VTX_UTIL PUBLIC ${GLM_INCLUDE_DIR})
target_include_directories(VTX_UTIL PRIVATE ${SPDLOG_INCLUDE_DIR}/include)
target_include_directories(VTX_UTIL PRIVATE src)
target_compile_features(VTX_UTIL PUBLIC cxx_std_17)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY ${GIT_URL_CATCH2}
  GIT_TAG        ${GIT_TAG_CATCH2}
)
FetchContent_MakeAvailable(Catch2)

add_executable(VTX_UTIL_TEST test/main.cpp)
target_link_libraries(VTX_UTIL_TEST PRIVATE Catch2::Catch2WithMain)
target_link_libraries(VTX_UTIL_TEST PRIVATE VTX_UTIL)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(VTX_UTIL_TEST)

