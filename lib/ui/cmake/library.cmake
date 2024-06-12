include("${CMAKE_CURRENT_LIST_DIR}/qt_helper.cmake")

configure_qt()
add_library(vtx_ui)
configure_target(vtx_ui)

set(QT_RESOURCES_PATH ${CMAKE_CURRENT_LIST_DIR}/../asset/qt/resources)

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_LIST_DIR}/../include/*)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/../src/*)
file(GLOB_RECURSE QT_RESOURCES ${QT_RESOURCES_PATH}/*)
target_sources(vtx_ui
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}/../include FILES ${HEADERS})
	
qt_add_resources(vtx_ui vtx_qt_resources_ui BASE ${QT_RESOURCES_PATH} FILES ${QT_RESOURCES})

if (NOT DEFINED _VTX_UI_CONAN)
	target_link_libraries(vtx_ui PUBLIC vtx_util)
	target_link_libraries(vtx_ui PUBLIC vtx_renderer)
	target_link_libraries(vtx_ui PUBLIC vtx_core)
	target_link_libraries(vtx_ui PUBLIC vtx_io)
	target_link_libraries(vtx_ui PUBLIC vtx_app)
	target_link_libraries(vtx_ui PUBLIC vtx_python_binding)
	target_link_libraries(vtx_ui PUBLIC PyTX)

else()
	target_link_libraries(vtx_ui PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_ui PRIVATE vtx_renderer::vtx_renderer)
	target_link_libraries(vtx_ui PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_ui PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_ui PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx_ui PRIVATE vtx_python_binding::vtx_python_binding)
	target_link_libraries(vtx_ui PRIVATE vtx_python_binding::PyTX)

endif()

target_link_libraries(vtx_ui PRIVATE Qt6::Core)
target_link_libraries(vtx_ui PRIVATE Qt6::Gui)
target_link_libraries(vtx_ui PRIVATE Qt6::Widgets)
target_link_libraries(vtx_ui PRIVATE Qt6::OpenGLWidgets)
	
if(MSVC)
	target_link_libraries(vtx_ui PRIVATE Qt6::QWindowsIntegrationPlugin)
endif()
