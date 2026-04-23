# Create the library.
add_library(vtx_tool_example)
add_library(vtx_tool_example::vtx_tool_example ALIAS vtx_tool_example)
vtx_configure_target(vtx_tool_example)

# Get files.
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")

# Link files.
target_sources(vtx_tool_example
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

# Add assets.
vtx_qt_add_resources(vtx_tool_example ${CMAKE_CURRENT_LIST_DIR}/../asset vtx_qt_resources_tool_example)

# Test exec.
file(GLOB_RECURSE SOURCES_TEST "${CMAKE_CURRENT_LIST_DIR}/../test/*")
add_executable(vtx_tool_example_test ${SOURCES_TEST})
set_property(TARGET vtx_tool_example_test PROPERTY FOLDER "test")
vtx_configure_target(vtx_tool_example_test)
if (VTX_RENDERER)
	vtx_link_cuda(vtx_tool_example_test)
endif()

# Link internal dependencies.
target_link_libraries(vtx_tool_example PUBLIC vtx_util::vtx_util)
target_link_libraries(vtx_tool_example PUBLIC vtx_app::vtx_app)
target_link_libraries(vtx_tool_example PUBLIC vtx_ui_qt::vtx_ui_qt)
target_link_libraries(vtx_tool_example_test PRIVATE vtx_app::vtx_app)

# Link tests.
target_link_libraries(vtx_tool_example_test PRIVATE vtx_tool_example::vtx_tool_example)
target_link_libraries(vtx_tool_example_test PRIVATE Catch2::Catch2WithMain)
if (VTX_PYTHON_BINDING)
	vtx_python_binding_copy_runtime(vtx_tool_example_test)
endif()
vtx_qt_copy_runtime(vtx_tool_example_test)

include(CTest)
include(Catch)
catch_discover_tests(vtx_tool_example_test DISCOVERY_MODE PRE_TEST)
