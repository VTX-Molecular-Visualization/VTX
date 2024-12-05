include("${CMAKE_CURRENT_LIST_DIR}/copy_shaders.cmake")

# Lib.
add_library(vtx_renderer)
configure_target(vtx_renderer)

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
#file(GLOB_RECURSE GLAD_HEADERS "${CMAKE_CURRENT_LIST_DIR}/../vendor/glad/include/*")
#file(GLOB_RECURSE GLAD_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../vendor/glad/src/*")
file(GLOB_RECURSE SHADERS "${CMAKE_CURRENT_LIST_DIR}/../shaders/*")
target_sources(vtx_renderer
	PRIVATE ${SOURCES}
	#PRIVATE ${GLAD_SOURCES}
	PRIVATE ${SHADERS}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS}
	#PUBLIC FILE_SET public_headers_glad TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../vendor/glad/include" FILES ${GLAD_HEADERS}
)

# Lib without opengl for CI.
add_library(vtx_renderer_no_opengl)
configure_target(vtx_renderer_no_opengl)
target_sources(vtx_renderer_no_opengl
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})
target_compile_definitions(vtx_renderer_no_opengl PRIVATE VTX_RENDERER_NO_OPENGL)

# Tests.
file(GLOB_RECURSE TESTS "${CMAKE_CURRENT_LIST_DIR}/../test/*")
add_executable(vtx_renderer_test ${TESTS})
configure_target(vtx_renderer_test)

if (NOT DEFINED _VTX_RENDERER_CONAN)
	target_link_libraries(vtx_renderer PRIVATE vtx_util)
	target_link_libraries(vtx_renderer_no_opengl PRIVATE vtx_util)
	target_link_libraries(vtx_renderer_test PRIVATE vtx_util)
else()
	target_link_libraries(vtx_renderer PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_renderer_no_opengl PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_renderer_test PRIVATE vtx_util::vtx_util)
endif()

target_link_libraries(vtx_renderer_test PRIVATE vtx_renderer_no_opengl)
target_link_libraries(vtx_renderer_test PRIVATE Catch2::Catch2WithMain)

vtx_copy_registered_data(vtx_renderer) # allow declared files to be copied on build

include(CTest)
include(Catch)
catch_discover_tests(vtx_renderer_test DISCOVERY_MODE PRE_TEST)