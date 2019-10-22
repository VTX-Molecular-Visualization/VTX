
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(mmtf-cpp VERSION 1.0.0 LANGUAGES CXX)

option(mmtf_build_local "Use the submodule dependencies for building" OFF)
option(mmtf_build_examples "Build the examples" OFF)

add_library(MMTFcpp INTERFACE)
target_compile_features(MMTFcpp INTERFACE cxx_auto_type)

target_include_directories(MMTFcpp INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (mmtf_build_local)
    # use header only
    set(MSGPACKC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/msgpack-c/include)
    add_library(msgpackc INTERFACE)
    target_include_directories(msgpackc INTERFACE ${MSGPACKC_INCLUDE_DIR})
    if (BUILD_TESTS)
        set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Catch2/single_include)
        add_library(Catch INTERFACE)
        target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})
    endif()
endif()

if (NOT TARGET msgpackc)
    find_package(msgpack)
endif()

target_link_libraries(MMTFcpp INTERFACE msgpackc)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()

if (mmtf_build_examples)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
endif()

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION  "include"
    FILES_MATCHING PATTERN "*.hpp")
