include("${CMAKE_CURRENT_LIST_DIR}/copy_shaders.cmake")

add_library(vtx_renderer)
configure_target(vtx_renderer)

set(HEADERS "")
set(SOURCES "")
set(SHADERS "")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
file(GLOB_RECURSE SHADERS "${CMAKE_CURRENT_LIST_DIR}/../shaders/*")
target_sources(vtx_renderer
	PRIVATE ${SOURCES}
	PRIVATE ${SHADERS}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

if (NOT DEFINED _VTX_RENDERER_CONAN)
	target_link_libraries(vtx_renderer vtx_util)
	target_link_libraries(vtx_renderer glad::glad)
else()
	target_link_libraries(vtx_renderer PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_renderer PRIVATE glad::glad)
endif()