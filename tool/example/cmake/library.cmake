# Lib.
add_library(vtx_tool_example)
configure_target(vtx_tool_example)

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
target_sources(vtx_tool_example
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

# Tests.
file(GLOB_RECURSE SOURCES_TEST "${CMAKE_CURRENT_LIST_DIR}/../test/*")
add_executable(vtx_tool_example_test ${SOURCES_TEST})
configure_target(vtx_tool_example_test)

if (NOT DEFINED _VTX_TOOL_EXAMPLE_CONAN)
	target_link_libraries(vtx_tool_example PRIVATE vtx_util)
	target_link_libraries(vtx_tool_example PRIVATE vtx_renderer)
	target_link_libraries(vtx_tool_example PRIVATE vtx_core)
	target_link_libraries(vtx_tool_example PRIVATE vtx_io)
	target_link_libraries(vtx_tool_example PRIVATE vtx_app)
	target_link_libraries(vtx_tool_example PRIVATE vtx_ui_qt)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_util)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_renderer_no_opengl)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_core)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_io)	
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_app)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_ui_qt)
else()
	target_link_libraries(vtx_tool_example PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_tool_example PRIVATE vtx_renderer::vtx_renderer)
	target_link_libraries(vtx_tool_example PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_tool_example PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_tool_example PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx_tool_example PRIVATE vtx_ui_qt::vtx_ui_qt)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_renderer::vtx_renderer_no_opengl)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_ui_qt::vtx_ui_qt)
endif()

target_link_libraries(vtx_tool_example PRIVATE Qt6::Core)
target_link_libraries(vtx_tool_example PRIVATE Qt6::Gui)
target_link_libraries(vtx_tool_example PRIVATE Qt6::Widgets)
target_link_libraries(vtx_tool_example PRIVATE Qt6::OpenGLWidgets)

target_link_libraries(vtx_tool_example_test PRIVATE vtx_tool_example)
target_link_libraries(vtx_tool_example_test PRIVATE Catch2::Catch2WithMain)

target_compile_definitions(vtx_tool_example_test PRIVATE VTX_RENDERER_NO_OPENGL)

catch_discover_tests(vtx_tool_example_test DISCOVERY_MODE PRE_TEST)