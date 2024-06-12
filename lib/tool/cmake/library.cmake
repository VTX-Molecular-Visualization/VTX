add_library(vtx_tool)
configure_target(vtx_tool)

set(QT_RESOURCES_PATH ${CMAKE_CURRENT_LIST_DIR}/../asset/qt/resources)

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_LIST_DIR}/../include/*)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/../src/*)
file(GLOB_RECURSE QT_RESOURCES ${QT_RESOURCES_PATH}/*)
target_sources(vtx_tool
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}/../include FILES ${HEADERS})

qt_add_resources(vtx_tool vtx_qt_resources_tool BASE ${QT_RESOURCES_PATH} FILES ${QT_RESOURCES})

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