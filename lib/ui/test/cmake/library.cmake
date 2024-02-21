add_executable(vtx_ui_test "${CMAKE_CURRENT_LIST_DIR}/../src/main.cpp")
configure_target(vtx_ui_test)

if (NOT DEFINED _VTX_UI_TEST_CONAN)
	target_link_libraries(vtx_ui_test PRIVATE vtx_util)
	target_link_libraries(vtx_ui_test PRIVATE vtx_core)
	target_link_libraries(vtx_ui_test PRIVATE vtx_renderer)
	target_link_libraries(vtx_ui_test PRIVATE vtx_io)
	target_link_libraries(vtx_ui_test PRIVATE vtx_app)
	target_link_libraries(vtx_ui_test PRIVATE vtx_python_binding)
	target_link_libraries(vtx_ui_test PRIVATE PyTX)
	target_link_libraries(vtx_ui_test PRIVATE vtx_ui)
else()
	target_link_libraries(vtx_ui_test PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_ui_test PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_ui_test PRIVATE vtx_renderer::vtx_renderer)
	target_link_libraries(vtx_ui_test PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_ui_test PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx_ui_test PRIVATE vtx_python_binding::vtx_python_binding)
	target_link_libraries(vtx_ui_test PRIVATE vtx_python_binding::PyTX)
	target_link_libraries(vtx_ui_test PRIVATE vtx_ui::vtx_ui)
endif()

target_link_libraries(vtx_ui_test PRIVATE pybind11::embed)
target_link_libraries(vtx_ui_test PRIVATE Catch2::Catch2WithMain)



add_custom_target(
	vtx_ui_test_copy_shader
	ALL COMMAND
	${CMAKE_COMMAND} -E copy_directory	
	${DIR_SHADERS}
	$<TARGET_FILE_DIR:vtx_ui_test>/shaders)
add_dependencies(vtx_ui_test vtx_ui_test_copy_shader)

add_custom_target(vtx_ui_copy_data ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:vtx_ui_test>/data)
add_dependencies(vtx_ui_test vtx_ui_copy_data)

include(CTest)
include(Catch)
catch_discover_tests(vtx_ui_test)
