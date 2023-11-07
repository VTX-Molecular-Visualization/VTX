message("vtx_tool")
cmake_minimum_required(VERSION 3.23)
include(../util/cmake/configure-target.cmake)
project(vtx_tool)

#include(cmake/qt_helper.cmake)

#configure_qt()

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_ui CONFIG REQUIRED)
find_package(Eigen3 REQUIRED)

file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)
file(GLOB_RECURSE QT_FORMS asset/qt/forms/*.ui)
file(GLOB_RECURSE QT_RESOURCES asset/qt/resources/*.qrc)

add_library(vtx_tool STATIC ${HEADERS} ${SOURCES} ${QT_FORMS} ${QT_RESOURCES})
configureTarget(vtx_tool)
target_include_directories(vtx_tool PUBLIC include)

target_link_libraries(vtx_tool PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_tool PRIVATE vtx_app::vtx_app)
target_link_libraries(vtx_tool PRIVATE vtx_ui::vtx_ui)
target_link_libraries(vtx_tool PRIVATE Eigen3::Eigen)
#target_link_libraries(vtx_tool PRIVATE Qt6::Core)
#target_link_libraries(vtx_tool PRIVATE Qt6::Gui)
#target_link_libraries(vtx_tool PRIVATE Qt6::Widgets)

#apply_qt_options(vtx_tool)