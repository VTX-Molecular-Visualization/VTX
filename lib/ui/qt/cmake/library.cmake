include("${CMAKE_CURRENT_LIST_DIR}/vtx_qt_configure.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/vtx_qt_add_resources.cmake")

vtx_qt_configure()
add_library(vtx_ui_qt)
vtx_configure_target(vtx_ui_qt)

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_LIST_DIR}/../include/*)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/../src/*)
target_sources(vtx_ui_qt
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}/../include FILES ${HEADERS})
	
vtx_qt_add_resources(vtx_ui_qt ${CMAKE_CURRENT_LIST_DIR}/../asset vtx_qt_resources_ui)

if (NOT DEFINED _VTX_UI_QT_CONAN)
	target_link_libraries(vtx_ui_qt PRIVATE vtx_util)
	target_link_libraries(vtx_ui_qt PRIVATE vtx_app)
	target_link_libraries(vtx_ui_qt PRIVATE vtx_renderer)
	target_link_libraries(vtx_ui_qt PRIVATE vtx_core)
	target_link_libraries(vtx_ui_qt PRIVATE vtx_python_binding)

else()
	target_link_libraries(vtx_ui_qt PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_ui_qt PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx_ui_qt PRIVATE vtx_renderer::vtx_renderer)
	target_link_libraries(vtx_ui_qt PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_ui_qt PRIVATE vtx_python_binding::vtx_python_binding)
endif()

target_link_libraries(vtx_ui_qt PRIVATE Qt6::Core)
target_link_libraries(vtx_ui_qt PRIVATE Qt6::Gui)
target_link_libraries(vtx_ui_qt PRIVATE Qt6::Widgets)
target_link_libraries(vtx_ui_qt PUBLIC EnTT::EnTT)  # TODO : see <lib/python_binding/cmake/library.cmake> for details
	
if(MSVC)
	target_link_libraries(vtx_ui_qt PRIVATE Qt6::QWindowsIntegrationPlugin)
endif()
