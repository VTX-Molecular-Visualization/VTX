message("vtx_tool")
cmake_minimum_required(VERSION 3.23)

project(vtx_tool)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_ui CONFIG REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(vtx_tool_mdprep REQUIRED)

configure_qt()
add_library(vtx_tool)
configure_target(vtx_tool)

file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)
file(GLOB_RECURSE QT_RESOURCES asset/qt/resources/*)
target_sources(vtx_tool
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS include FILES ${HEADERS})

qt_add_resources(
	vtx_tool
	vtx_qt_resources_tool
	BASE asset/qt/resources/
    FILES ${QT_RESOURCES}
)

target_link_libraries(vtx_tool PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_tool PRIVATE vtx_app::vtx_app)
target_link_libraries(vtx_tool PRIVATE vtx_ui::vtx_ui)
target_link_libraries(vtx_tool PRIVATE Eigen3::Eigen)
target_link_libraries(vtx_tool PRIVATE vtx_tool_mdprep::vtx_tool_mdprep)

install(TARGETS vtx_tool FILE_SET public_headers DESTINATION include)