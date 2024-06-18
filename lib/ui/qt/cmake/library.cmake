include("${CMAKE_CURRENT_LIST_DIR}/qt_helper.cmake")

configure_qt()
add_library(vtx_ui_qt)
configure_target(vtx_ui_qt)

set(QT_RESOURCES_PATH ${CMAKE_CURRENT_LIST_DIR}/../asset)

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_LIST_DIR}/../include/*)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/../src/*)
file(GLOB_RECURSE QT_RESOURCES ${QT_RESOURCES_PATH}/*)
target_sources(vtx_ui_qt
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}/../include FILES ${HEADERS})
	
qt_add_resources(vtx_ui_qt vtx_qt_resources_ui BASE ${QT_RESOURCES_PATH} FILES ${QT_RESOURCES})

if (NOT DEFINED _VTX_UI_QT_CONAN)
	target_link_libraries(vtx_ui_qt PUBLIC vtx_util)
	target_link_libraries(vtx_ui_qt PUBLIC vtx_ui)

else()
	target_link_libraries(vtx_ui_qt PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_ui_qt PRIVATE vtx_ui::vtx_ui)
endif()

target_link_libraries(vtx_ui_qt PRIVATE Qt6::Core)
target_link_libraries(vtx_ui_qt PRIVATE Qt6::Gui)
target_link_libraries(vtx_ui_qt PRIVATE Qt6::Widgets)
target_link_libraries(vtx_ui_qt PRIVATE Qt6::OpenGLWidgets)
	
if(MSVC)
	target_link_libraries(vtx_ui_qt PRIVATE Qt6::QWindowsIntegrationPlugin)
endif()
