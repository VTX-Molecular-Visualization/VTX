add_executable(vtx_tool_test ${CMAKE_CURRENT_LIST_DIR}/../src/main.cpp)
configure_target(vtx_tool_test)

if (NOT DEFINED _VTX_TOOL_TEST_CONAN)
	target_link_libraries(vtx_tool_test PRIVATE vtx_tool)
else()
	target_link_libraries(vtx_tool_test PRIVATE vtx_tool::vtx_tool)
endif()

target_link_libraries(vtx_tool_test PRIVATE Eigen3::Eigen)
target_link_libraries(vtx_tool_test PRIVATE Catch2::Catch2WithMain)
target_compile_definitions(vtx_tool_test PRIVATE VTX_RENDERER_NO_OPENGL)

catch_discover_tests(vtx_tool_test DISCOVERY_MODE PRE_TEST)
