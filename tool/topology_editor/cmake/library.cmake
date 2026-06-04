# Create the library.
add_library(vtx_tool_topology_editor)
add_library(vtx_tool_topology_editor::vtx_tool_topology_editor ALIAS vtx_tool_topology_editor)
vtx_configure_target(vtx_tool_topology_editor)

# Get files.
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")

# Link files.
target_sources(vtx_tool_topology_editor
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

# Test exec.
file(GLOB_RECURSE SOURCES_TEST "${CMAKE_CURRENT_LIST_DIR}/../test/*")
add_executable(vtx_tool_topology_editor_test ${SOURCES_TEST})
set_property(TARGET vtx_tool_topology_editor_test PROPERTY FOLDER "test")
vtx_configure_target(vtx_tool_topology_editor_test)

# Link internal dependencies.
target_link_libraries(vtx_tool_topology_editor PUBLIC vtx_util::vtx_util)
target_link_libraries(vtx_tool_topology_editor PUBLIC vtx_app::vtx_app)
target_link_libraries(vtx_tool_topology_editor PUBLIC vtx_ui_qt::vtx_ui_qt)
target_link_libraries(vtx_tool_topology_editor_test PRIVATE vtx_app::vtx_app)

# Link tests.
target_link_libraries(vtx_tool_topology_editor_test PRIVATE vtx_tool_topology_editor::vtx_tool_topology_editor)
target_link_libraries(vtx_tool_topology_editor_test PRIVATE Catch2::Catch2)

if (VTX_RENDERER)
	vtx_link_cuda(vtx_tool_topology_editor_test)
endif()

if (VTX_PYTHON_BINDING)
	vtx_python_binding_copy_runtime(vtx_tool_topology_editor_test)
endif()

vtx_qt_copy_runtime(vtx_tool_topology_editor_test)

include(CTest)
add_test(NAME vtx_tool_topology_editor_test COMMAND vtx_tool_topology_editor_test)
