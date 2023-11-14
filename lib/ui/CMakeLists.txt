message("vtx_ui")
cmake_minimum_required(VERSION 3.23)
include(cmake/qt_helper.cmake)
project(vtx_ui)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
#find_package(Qt6 COMPONENTS Svg REQUIRED)
#find_package(Qt6 COMPONENTS Gif REQUIRED)
#find_package(Qt6 COMPONENTS Jpeg REQUIRED)

configure_qt()
add_library(vtx_ui)
configure_target(vtx_ui)

file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)
file(GLOB_RECURSE QT_FORMS asset/qt/forms/*.ui)
file(GLOB_RECURSE QT_RESOURCES asset/qt/resources/*.qrc)
target_sources(vtx_ui
	PRIVATE ${SOURCES}
	PRIVATE ${QT_FORMS}
	PRIVATE ${QT_RESOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS include FILES ${HEADERS})

target_link_libraries(vtx_ui PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_ui PRIVATE vtx_renderer::vtx_renderer)
target_link_libraries(vtx_ui PRIVATE vtx_core::vtx_core)
target_link_libraries(vtx_ui PRIVATE vtx_io::vtx_io)
target_link_libraries(vtx_ui PRIVATE vtx_app::vtx_app)
target_link_libraries(vtx_ui PRIVATE Qt6::Core)
target_link_libraries(vtx_ui PRIVATE Qt6::Gui)
target_link_libraries(vtx_ui PRIVATE Qt6::Widgets)
target_link_libraries(vtx_ui PRIVATE Qt6::OpenGLWidgets)
#target_link_libraries(vtx_ui PRIVATE Qt6::Svg)
#target_link_libraries(vtx_ui PRIVATE Qt6::Gif)
#target_link_libraries(vtx_ui PRIVATE Qt6::Jpeg)
if(MSVC)
	target_link_libraries(vtx_ui PRIVATE Qt6::QWindowsIntegrationPlugin)
endif()

apply_qt_options(vtx_ui)

install(TARGETS vtx_ui FILE_SET public_headers DESTINATION include)