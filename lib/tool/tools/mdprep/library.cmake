
include ("${CMAKE_CURRENT_LIST_DIR}/cmake/vtx_tool_mdprep_copy_files.cmake")

#add_library(vtx_tool_mdprep)
#configure_target(vtx_tool_mdprep)
#
#file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/include/*")
#file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*")
#file(GLOB_RECURSE QT_FORMS asset/qt/forms/*.ui)
#file(GLOB_RECURSE QT_RESOURCES asset/qt/resources/*.qrc)
#target_sources(vtx_tool_mdprep
#	PRIVATE ${SOURCES}
#	PRIVATE ${QT_FORMS}
#	PRIVATE ${QT_RESOURCES}
#	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS include FILES ${HEADERS})
#
#target_link_libraries(vtx_tool_mdprep PRIVATE vtx_util::vtx_util)
#target_link_libraries(vtx_tool_mdprep PRIVATE vtx_core::vtx_core)
#target_link_libraries(vtx_tool_mdprep PRIVATE vtx_app::vtx_app)
#target_link_libraries(vtx_tool_mdprep PRIVATE Qt6::Core)
#target_link_libraries(vtx_tool_mdprep PRIVATE Qt6::Gui)
#target_link_libraries(vtx_tool_mdprep PRIVATE Qt6::Widgets)
#target_link_libraries(vtx_tool_mdprep PRIVATE Qt6::OpenGLWidgets)