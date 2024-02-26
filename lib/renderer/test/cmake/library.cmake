add_executable(vtx_renderer_test "${CMAKE_CURRENT_LIST_DIR}/../src/main.cpp")
configure_target(vtx_renderer_test)

if (NOT DEFINED _VTX_RENDERER_TEST_CONAN)
	target_link_libraries(vtx_renderer_test vtx_util)
	target_link_libraries(vtx_renderer_test vtx_renderer)
	target_link_libraries(vtx_renderer_test Catch2::Catch2WithMain)

else()
	target_link_libraries(vtx_renderer_test PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_renderer_test PRIVATE vtx_renderer::vtx_renderer)
	target_link_libraries(vtx_renderer_test PRIVATE Catch2::Catch2WithMain)
endif()

vtx_copy_registered_data(vtx_renderer_test)
catch_discover_tests(vtx_renderer_test)