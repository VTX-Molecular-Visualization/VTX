include("${CMAKE_CURRENT_LIST_DIR}/qt_helper.cmake")

configure_qt()
add_library(vtx_ui)
configure_target(vtx_ui)

find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
set(HEADERS "")
set(SOURCES "")
set(QT_FORMS "")
set(QT_RESOURCES "")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
file(GLOB_RECURSE QT_FORMS "${CMAKE_CURRENT_LIST_DIR}/../asset/qt/forms/*.ui")
file(GLOB_RECURSE QT_RESOURCES "${CMAKE_CURRENT_LIST_DIR}/../asset/qt/resources/*.qrc")
target_sources(vtx_ui
	PRIVATE ${SOURCES}
	PRIVATE ${QT_FORMS}
	PRIVATE ${QT_RESOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

if (NOT DEFINED _VTX_UI_CONAN)
	message("VTX - UI - entring <NOT DEFINED _VTX_UI_CONAN>")
	target_link_libraries(vtx_ui PRIVATE vtx_util)
	target_link_libraries(vtx_ui PRIVATE vtx_renderer)
	target_link_libraries(vtx_ui PRIVATE vtx_core)
	target_link_libraries(vtx_ui PRIVATE vtx_io)
	target_link_libraries(vtx_ui PRIVATE vtx_app)
	target_link_libraries(vtx_ui PRIVATE vtx_python_binding)
	target_link_libraries(vtx_ui PRIVATE PyTX)
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

apply_qt_options(vtx_ui)
