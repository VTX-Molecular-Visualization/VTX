include(CheckLanguage)
include("${CMAKE_CURRENT_LIST_DIR}/copy_shaders.cmake")

# Lib.
add_library(vtx_renderer)
configure_target(vtx_renderer)

file(GLOB_RECURSE HEADERS_PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../include/public/*")
file(GLOB_RECURSE HEADERS_PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../include/private/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
file(GLOB_RECURSE HEADERS_VENDORS "${CMAKE_CURRENT_LIST_DIR}/../vendor/*.h")
file(GLOB_RECURSE SOURCES_VENDORS "${CMAKE_CURRENT_LIST_DIR}/../vendor/*.c")
file(GLOB_RECURSE SHADERS "${CMAKE_CURRENT_LIST_DIR}/../shaders/*")
target_sources(vtx_renderer
	PRIVATE ${SOURCES}
	PRIVATE ${SOURCES_VENDORS}
	PRIVATE ${SHADERS}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include/public" FILES ${HEADERS_PUBLIC}
	PRIVATE FILE_SET private_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include/private" FILES ${HEADERS_PRIVATE}
	PRIVATE FILE_SET vendors_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../vendor" FILES ${HEADERS_VENDORS}
)

# Cuda.
# Print message if CUDA is not found.
check_language(CUDA)
if (CMAKE_CUDA_COMPILER)
	message(STATUS "CUDA found")
	enable_language(CUDA)
	#file(GLOB_RECURSE CUDA_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/cuda/*")
	#target_sources(vtx_renderer PRIVATE ${CUDA_SOURCES})
else()
	message(STATUS "CUDA not found")
endif()

# Tests.
file(GLOB_RECURSE TESTS "${CMAKE_CURRENT_LIST_DIR}/../test/*")
add_executable(vtx_renderer_test ${TESTS})
configure_target(vtx_renderer_test)

if (NOT DEFINED _VTX_RENDERER_CONAN)
	target_link_libraries(vtx_renderer PRIVATE vtx_util)
	target_link_libraries(vtx_renderer_test PRIVATE vtx_util)
else()
	target_link_libraries(vtx_renderer PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_renderer_test PRIVATE vtx_util::vtx_util)
endif()

target_link_libraries(vtx_renderer_test PRIVATE vtx_renderer)
target_link_libraries(vtx_renderer_test PRIVATE Catch2::Catch2WithMain)

vtx_copy_registered_data(vtx_renderer) # allow declared files to be copied on build

include(CTest)
include(Catch)
catch_discover_tests(vtx_renderer_test DISCOVERY_MODE PRE_TEST)