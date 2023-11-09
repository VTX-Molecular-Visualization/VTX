message("vtx_tool")
cmake_minimum_required(VERSION 3.23)
include(cmake/configure-target.cmake)
include(cmake/qt_helper.cmake)
project(vtx_tool)

configure_qt()

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_ui CONFIG REQUIRED)
#find_package(Qt6 COMPONENTS Core REQUIRED)
#find_package(Qt6 COMPONENTS Gui REQUIRED)
#find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Eigen3 REQUIRED)

add_library(vtx_tool)
configureTarget(vtx_tool)

file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)
file(GLOB_RECURSE QT_FORMS asset/qt/forms/*.ui)
file(GLOB_RECURSE QT_RESOURCES asset/qt/resources/*.qrc)
target_sources(vtx_tool
	PRIVATE ${SOURCES}
	PRIVATE ${QT_FORMS}
	PRIVATE ${QT_RESOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS include FILES ${HEADERS})

target_link_libraries(vtx_tool PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_tool PRIVATE vtx_app::vtx_app)
target_link_libraries(vtx_tool PRIVATE vtx_ui::vtx_ui)
target_link_libraries(vtx_tool PRIVATE Eigen3::Eigen)
#target_link_libraries(vtx_tool PRIVATE Qt6::Core)
#target_link_libraries(vtx_tool PRIVATE Qt6::Gui)
#target_link_libraries(vtx_tool PRIVATE Qt6::Widgets)

apply_qt_options(vtx_tool)

install(TARGETS vtx_tool FILE_SET public_headers DESTINATION include)