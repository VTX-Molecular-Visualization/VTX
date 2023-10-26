message("vtx_ui")
cmake_minimum_required(VERSION 3.23)
project(vtx_ui)

include(cmake/qt_helper.cmake)

configure_qt()

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
find_package(Qt6 COMPONENTS Network REQUIRED)
find_package(Qt6 COMPONENTS Svg REQUIRED)
find_package(Qt6QGifPlugin PATHS ${Qt6Gui_DIR}) # Workaround
find_package(Qt6QJpegPlugin PATHS ${Qt6Gui_DIR}) # Workaround
if(WIN32)
    find_package(Qt6QWindowsIntegrationPlugin PATHS ${Qt6Gui_DIR}) # Workaround
    find_package(Qt6QSchannelBackendPlugin PATHS ${Qt6Network_DIR}) # Workaround
endif()

file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)
file(GLOB_RECURSE QT_FORMS asset/qt/forms/*.ui)
file(GLOB_RECURSE QT_RESOURCES asset/qt/resources/*.qrc)

add_library(vtx_ui STATIC ${HEADERS} ${SOURCES} ${QT_FORMS} ${QT_RESOURCES})
target_include_directories(vtx_ui PUBLIC include)

target_link_libraries(vtx_app PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_app PRIVATE vtx_renderer::vtx_renderer)
target_link_libraries(vtx_app PRIVATE vtx_core::vtx_core)
target_link_libraries(vtx_app PRIVATE vtx_io::vtx_io)
target_link_libraries(vtx_app PRIVATE vtx_app::vtx_app)
target_link_libraries(vtx_ui PRIVATE Qt6::Core)
target_link_libraries(vtx_ui PRIVATE Qt6::Gui)
target_link_libraries(vtx_ui PRIVATE Qt6::Widgets)
target_link_libraries(vtx_ui PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(vtx_ui PRIVATE Qt6::Network)
target_link_libraries(vtx_ui PRIVATE Qt6::Svg)

apply_qt_options(vtx_ui)