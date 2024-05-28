
include ("${CMAKE_CURRENT_LIST_DIR}/vtx_tool_mdprep_copy_files.cmake")

add_library(vtx_tool_mdprep)
configure_target(vtx_tool_mdprep)

set(HEADERS "")
set(SOURCES "")
set(QT_FORMS "")
set(QT_RESOURCES "")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")

target_sources(vtx_tool_mdprep
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})


if (NOT DEFINED _VTX_MDPREP_CONAN)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_util)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_core)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_app)
else()
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_app::vtx_app)
endif()
target_link_libraries(vtx_tool_mdprep PRIVATE Qt6::Core)
target_link_libraries(vtx_tool_mdprep PRIVATE Qt6::Gui)
target_link_libraries(vtx_tool_mdprep PRIVATE Qt6::Widgets)
target_link_libraries(vtx_tool_mdprep PRIVATE Qt6::OpenGLWidgets)

vtx_copy_registered_data(vtx_tool_mdprep)