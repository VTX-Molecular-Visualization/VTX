message("vtx_tool_mdprep")
cmake_minimum_required(VERSION 3.23)

project(vtx_tool_mdprep)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)

add_library(vtx_tool_mdprep)
configure_target(vtx_tool_mdprep)

file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)
file(GLOB_RECURSE QT_FORMS asset/qt/forms/*.ui)
file(GLOB_RECURSE QT_RESOURCES asset/qt/resources/*.qrc)
target_sources(vtx_tool_mdprep
	PRIVATE ${SOURCES}
	PRIVATE ${QT_FORMS}
	PRIVATE ${QT_RESOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS include FILES ${HEADERS})

target_link_libraries(vtx_tool_mdprep PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_tool_mdprep PRIVATE vtx_core::vtx_core)
target_link_libraries(vtx_tool_mdprep PRIVATE vtx_app::vtx_app)
target_link_libraries(vtx_tool_mdprep PRIVATE chemfiles::chemfiles)

install(TARGETS vtx_tool_mdprep FILE_SET public_headers)
