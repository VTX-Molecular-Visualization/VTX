

add_library(vtx_tool_structalign)
add_library(vtx_tool_structalign::vtx_tool_structalign ALIAS vtx_tool_structalign)
vtx_configure_target(vtx_tool_structalign)

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")

target_sources(vtx_tool_structalign
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

vtx_qt_add_resources(vtx_tool_structalign ${CMAKE_CURRENT_LIST_DIR}/../asset vtx_qt_resources_tool_mdprep)

target_link_libraries(vtx_tool_structalign PUBLIC vtx_util::vtx_util)
target_link_libraries(vtx_tool_structalign PUBLIC vtx_core::vtx_core)
target_link_libraries(vtx_tool_structalign PUBLIC vtx_app::vtx_app)
target_link_libraries(vtx_tool_structalign PUBLIC vtx_ui_qt::vtx_ui_qt)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../test/src/*")

add_executable(vtx_tool_structalign_test ${SOURCES} )
set_property(TARGET vtx_tool_structalign_test PROPERTY FOLDER "test")
vtx_configure_target(vtx_tool_structalign_test)

target_include_directories(vtx_tool_structalign_test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../include")

target_link_libraries(vtx_tool_structalign_test PRIVATE vtx_app::vtx_app)
target_link_libraries(vtx_tool_structalign_test PRIVATE vtx_tool_structalign::vtx_tool_structalign)
target_link_libraries(vtx_tool_structalign_test PRIVATE Catch2::Catch2)
target_link_libraries(vtx_tool_structalign_test PRIVATE re2::re2)

if (VTX_RENDERER)
	vtx_link_cuda(vtx_tool_structalign_test)
endif()

if (VTX_PYTHON_BINDING)
	vtx_python_binding_copy_runtime(vtx_tool_structalign_test)
endif()

vtx_qt_copy_runtime(vtx_tool_structalign_test)
vtx_tool_mdprep_copy_runtime(vtx_tool_structalign_test)

include(CTest)
add_test(NAME vtx_tool_structalign_test COMMAND vtx_tool_structalign_test)
vtx_tool_mdprep_copy_data(vtx_tool_structalign_test)
vtx_copy_directory(vtx_tool_structalign_test "${CMAKE_CURRENT_LIST_DIR}/../test/data" "$<TARGET_FILE_DIR:vtx_tool_structalign_test>/data")
