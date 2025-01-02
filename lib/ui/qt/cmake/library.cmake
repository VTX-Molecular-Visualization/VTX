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
	
add_resources(vtx_ui_qt ${CMAKE_CURRENT_LIST_DIR}/../asset vtx_qt_resources_ui)

if (NOT DEFINED _VTX_UI_QT_CONAN)
	target_link_libraries(vtx_ui_qt PRIVATE vtx_util)
	target_link_libraries(vtx_ui_qt PRIVATE vtx_app)
	target_link_libraries(vtx_ui_qt PRIVATE vtx_renderer)
	target_link_libraries(vtx_ui_qt PRIVATE vtx_core)

else()
	target_link_libraries(vtx_ui_qt PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_ui_qt PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx_ui_qt PRIVATE vtx_renderer::vtx_renderer)
	target_link_libraries(vtx_ui_qt PRIVATE vtx_core::vtx_core)
endif()

target_link_libraries(vtx_ui_qt PRIVATE Qt6::Core)
target_link_libraries(vtx_ui_qt PRIVATE Qt6::Gui)
target_link_libraries(vtx_ui_qt PRIVATE Qt6::Widgets)
target_link_libraries(vtx_ui_qt PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(vtx_ui_qt PUBLIC EnTT::EnTT)  # TODO : see <lib/python_binding/cmake/library.cmake> for details
	
if(MSVC)
	target_link_libraries(vtx_ui_qt PRIVATE Qt6::QWindowsIntegrationPlugin)
endif()
