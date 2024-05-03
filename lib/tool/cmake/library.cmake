configure_qt()
add_library(vtx_tool)
configure_target(vtx_tool)

set(HEADERS "")
set(SOURCES "")
set(QT_FORMS "")
set(QT_RESOURCES "")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
file(GLOB_RECURSE QT_FORMS asset/qt/forms/*.ui)
file(GLOB_RECURSE QT_RESOURCES asset/qt/resources/*.qrc)
target_sources(vtx_tool
	PRIVATE ${SOURCES}
	PRIVATE ${QT_FORMS}
	PRIVATE ${QT_RESOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

if (NOT DEFINED _VTX_TOOL_CONAN)
	target_link_libraries(vtx_tool PRIVATE vtx_util)
	target_link_libraries(vtx_tool PRIVATE vtx_app)
	target_link_libraries(vtx_tool PRIVATE vtx_ui)
	target_link_libraries(vtx_tool PRIVATE vtx_tool_mdprep)
else()
	target_link_libraries(vtx_tool PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_tool PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx_tool PRIVATE vtx_ui::vtx_ui)
	target_link_libraries(vtx_tool PRIVATE vtx_tool_mdprep::vtx_tool_mdprep)
endif()
target_link_libraries(vtx_tool PRIVATE Eigen3::Eigen)

apply_qt_options(vtx_tool)
