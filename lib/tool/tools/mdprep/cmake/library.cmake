
include ("${CMAKE_CURRENT_LIST_DIR}/vtx_tool_mdprep_copy_files.cmake")

add_library(vtx_tool_mdprep)
configure_target(vtx_tool_mdprep)

set(HEADERS "")
set(SOURCES "")
set(QT_FORMS "")
set(QT_RESOURCES "")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
message("mdprep headers : <${HEADERS}>")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
file(GLOB_RECURSE QT_FORMS asset/qt/forms/*.ui)
file(GLOB_RECURSE QT_RESOURCES asset/qt/resources/*.qrc)
target_sources(vtx_tool_mdprep
	PRIVATE ${SOURCES}
	PRIVATE ${QT_FORMS}
	PRIVATE ${QT_RESOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})


if (NOT DEFINED _VTX_MDPREP_CONAN)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_util)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_core)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_app)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_ui)
else()
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_ui::vtx_ui)
endif()
target_link_libraries(vtx_tool_mdprep PRIVATE Qt6::Core)
target_link_libraries(vtx_tool_mdprep PRIVATE Qt6::Gui)
target_link_libraries(vtx_tool_mdprep PRIVATE Qt6::Widgets)
target_link_libraries(vtx_tool_mdprep PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(vtx_tool_mdprep PRIVATE re2::re2)

vtx_copy_registered_data(vtx_tool_mdprep)