# Create the library.
#configure_qt()
add_library(vtx_tool_example)
configure_target(vtx_tool_example)

# Get files.
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")

# Link files.
target_sources(vtx_tool_example
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

# Add assets.
add_resources(vtx_tool_example ${CMAKE_CURRENT_LIST_DIR}/../asset z_vtx_tool_example)

# Test exec.
file(GLOB_RECURSE SOURCES_TEST "${CMAKE_CURRENT_LIST_DIR}/../test/*")
add_executable(vtx_tool_example_test "${SOURCES_TEST};${SOURCES}")
target_include_directories(vtx_tool_example_test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../include")
configure_target(vtx_tool_example_test)

# Link internal dependencies.
if (NOT DEFINED _VTX_TOOL_EXAMPLE_CONAN)
	target_link_libraries(vtx_tool_example PRIVATE vtx_util)
	target_link_libraries(vtx_tool_example PRIVATE vtx_renderer)
	target_link_libraries(vtx_tool_example PRIVATE vtx_core)
	target_link_libraries(vtx_tool_example PRIVATE vtx_io)
	target_link_libraries(vtx_tool_example PRIVATE vtx_app)
	target_link_libraries(vtx_tool_example PRIVATE vtx_ui_qt)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_util)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_core)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_io)	
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_app)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_ui_qt)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_renderer_no_opengl)
else()
	target_link_libraries(vtx_tool_example PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_tool_example PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_tool_example PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_tool_example PRIVATE vtx_renderer::vtx_renderer)
	target_link_libraries(vtx_tool_example PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx_tool_example PRIVATE vtx_ui_qt::vtx_ui_qt)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_util::vtx_util)
	# It seems that, for now, adding the link toward these following packages confuses linux linker. Weird.
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_renderer::vtx_renderer_no_opengl)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_ui_qt::vtx_ui_qt)
	target_link_libraries(vtx_tool_example_test PRIVATE vtx_app::vtx_app_no_opengl)
endif()

# Link external dependencies.
target_link_libraries(vtx_tool_example PRIVATE Qt6::Core)
target_link_libraries(vtx_tool_example PRIVATE Qt6::Gui)
target_link_libraries(vtx_tool_example PRIVATE Qt6::Widgets)
target_link_libraries(vtx_tool_example PRIVATE Qt6::OpenGLWidgets)
# Link tests.
# target_link_libraries(vtx_tool_example_test PRIVATE vtx_tool_example)
target_link_libraries(vtx_tool_example_test PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(vtx_tool_example_test DISCOVERY_MODE PRE_TEST)