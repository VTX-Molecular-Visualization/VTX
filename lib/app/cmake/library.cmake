add_library(vtx_app)
configure_target(vtx_app)

set(SOURCES "")
set(HEADERS "")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
target_sources(vtx_app
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

if (NOT DEFINED _VTX_APP_CONAN)
	target_link_libraries(vtx_app PUBLIC vtx_util)
	target_link_libraries(vtx_app PUBLIC vtx_renderer)
	target_link_libraries(vtx_app PUBLIC vtx_core)
	target_link_libraries(vtx_app PUBLIC vtx_io)
	target_link_libraries(vtx_app PUBLIC EnTT::EnTT)
else()
	target_link_libraries(vtx_app PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_app PRIVATE vtx_renderer::vtx_renderer)
	target_link_libraries(vtx_app PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_app PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_app PRIVATE EnTT::EnTT)
endif()
