message("VTX_PYTHON_BINDING")
cmake_minimum_required(VERSION 3.20)
include(../util/cmake/configure-target.cmake)
project(VTX_PYTHON_BINDING)

# Git dependencies.
set(GIT_URL_PYBIND11 https://github.com/pybind/pybind11.git)
set(GIT_TAG_PYBIND11 v2.11.1)

# Fetch third parties.
include(FetchContent)
FetchContent_Declare(pybind11 GIT_REPOSITORY ${GIT_URL_PYBIND11} GIT_TAG ${GIT_TAG_PYBIND11})

FetchContent_MakeAvailable(pybind11)

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

# Create library.
add_library(VTX_PYTHON_BINDING STATIC ${HEADERS} ${SOURCES})
configureTarget(VTX_PYTHON_BINDING)
target_include_directories(VTX_PYTHON_BINDING PUBLIC include)


# Link VTX modules.
if(NOT TARGET VTX_UTIL)
	add_subdirectory(../util lib/util)
endif()
if(NOT TARGET VTX_CORE)
	add_subdirectory(../core lib/core)
endif()
if(NOT TARGET VTX_APP)
	add_subdirectory(../app lib/app)
endif()
target_link_libraries(VTX_PYTHON_BINDING PRIVATE VTX_UTIL)
target_link_libraries(VTX_PYTHON_BINDING PRIVATE VTX_CORE)
target_link_libraries(VTX_PYTHON_BINDING PRIVATE VTX_APP)

# Shared library PyTX generated from static library VTX_PYTHON_BINDING => static library must be position independent.
# it seems to be done automatically with MSVC and Clang but not with GCC
if(CMAKE_COMPILER_IS_GNUCC)
	#set_target_properties(VTX_PYTHON_BINDING PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
	target_compile_options(spdlog PRIVATE "-fPIC")
	target_compile_options(chemfiles PRIVATE "-fPIC")
	target_compile_options(VTX_UTIL PRIVATE "-fPIC")
	target_compile_options(VTX_CORE PRIVATE "-fPIC")
	target_compile_options(VTX_RENDERER PRIVATE "-fPIC")
	target_compile_options(VTX_APP PRIVATE "-fPIC")
	target_compile_options(VTX_PYTHON_BINDING PRIVATE "-fPIC")
endif()

# Link third parties.
target_link_libraries(VTX_PYTHON_BINDING PRIVATE pybind11::pybind11 pybind11::embed)
pybind11_add_module(PyTX SHARED "src/python_binding/python_module/global.cpp")

target_link_libraries(PyTX PRIVATE VTX_UTIL)
target_link_libraries(PyTX PRIVATE VTX_CORE)
target_link_libraries(PyTX PRIVATE VTX_APP)
target_link_libraries(PyTX PRIVATE VTX_PYTHON_BINDING)

## Test.
file(GLOB_RECURSE TEST_SOURCES test/*.hpp test/*.cpp)
add_executable(VTX_PYTHON_BINDING_TEST ${TEST_SOURCES})
configureTarget(VTX_PYTHON_BINDING_TEST)

# Link third parties.
target_link_libraries(VTX_PYTHON_BINDING_TEST PRIVATE Catch2::Catch2WithMain)

# Link libs.
target_link_libraries(VTX_PYTHON_BINDING_TEST PRIVATE VTX_UTIL)
target_link_libraries(VTX_PYTHON_BINDING_TEST PRIVATE VTX_IO)
target_link_libraries(VTX_PYTHON_BINDING_TEST PRIVATE VTX_CORE)
target_link_libraries(VTX_PYTHON_BINDING_TEST PRIVATE VTX_APP)
target_link_libraries(VTX_PYTHON_BINDING_TEST PRIVATE VTX_PYTHON_BINDING)
target_link_libraries(VTX_PYTHON_BINDING_TEST PRIVATE EnTT::EnTT)
target_link_libraries(VTX_PYTHON_BINDING_TEST PRIVATE PyTX)


# Catch2.
include(CTest)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)	
	catch_discover_tests(VTX_PYTHON_BINDING_TEST)
endif()

# Copy data.
add_custom_target(VTX_PYTHON_BINDING_TEST_COPY_DATA ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/test/data $<TARGET_FILE_DIR:VTX_PYTHON_BINDING_TEST>/data)
add_dependencies(VTX_PYTHON_BINDING_TEST VTX_PYTHON_BINDING_TEST_COPY_DATA)