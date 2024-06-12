file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/../src/*)
if(MSVC)
	file(GLOB_RECURSE RESOURCES ${CMAKE_CURRENT_LIST_DIR}/../asset/windows_resources/*.rc)
endif()

add_executable(vtx ${SOURCES} ${RESOURCES})
configure_target(vtx)

if (NOT DEFINED _VTX_CONAN)
	target_link_libraries(vtx PUBLIC vtx_util)
	target_link_libraries(vtx PUBLIC vtx_core)
	target_link_libraries(vtx PUBLIC vtx_renderer)
	target_link_libraries(vtx PUBLIC vtx_io)
	target_link_libraries(vtx PUBLIC vtx_app)
	target_link_libraries(vtx PUBLIC vtx_python_binding)
	target_link_libraries(vtx PUBLIC vtx_ui)
	target_link_libraries(vtx PUBLIC vtx_tool)
else()
	target_link_libraries(vtx PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx PRIVATE vtx_renderer::vtx_renderer)
	target_link_libraries(vtx PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx PRIVATE vtx_python_binding::vtx_python_binding)
	target_link_libraries(vtx PRIVATE vtx_ui::vtx_ui)
	target_link_libraries(vtx PRIVATE vtx_tool::vtx_tool)
endif()

vtx_register_build_directory_copy("${CMAKE_CURRENT_LIST_DIR}/../data" "./data")
vtx_copy_registered_data(vtx)
