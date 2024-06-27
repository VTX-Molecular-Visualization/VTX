add_library(vtx_ui)
configure_target(vtx_ui)

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_LIST_DIR}/../include/*)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/../src/*)
target_sources(vtx_ui
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}/../include FILES ${HEADERS})

if (NOT DEFINED _VTX_UI_CONAN)
	target_link_libraries(vtx_ui PUBLIC vtx_util)
	target_link_libraries(vtx_ui PUBLIC vtx_renderer)
	target_link_libraries(vtx_ui PUBLIC vtx_core)
	target_link_libraries(vtx_ui PUBLIC vtx_io)
	target_link_libraries(vtx_ui PUBLIC vtx_app)
else()
	target_link_libraries(vtx_ui PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_ui PRIVATE vtx_renderer::vtx_renderer)
	target_link_libraries(vtx_ui PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_ui PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_ui PRIVATE vtx_app::vtx_app)
endif()
