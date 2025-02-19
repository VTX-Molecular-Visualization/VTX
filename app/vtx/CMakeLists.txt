message("vtx")
cmake_minimum_required(VERSION 3.23)

project(vtx)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_ui_qt CONFIG REQUIRED)
find_package(vtx_tool_example CONFIG REQUIRED)
find_package(vtx_tool_mdprep CONFIG REQUIRED)

find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)

set(_VTX_CONAN " ")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/library.cmake")

install(TARGETS vtx)
install(DIRECTORY ${DIR_SHADERS} DESTINATION bin)
install(DIRECTORY ${DIR_PYTHON_SCRIPT} DESTINATION bin)
install(FILES ${PATH_PYTHON_MODULE} DESTINATION bin RENAME ${FILENAME_PYTHON_MODULE})
install(DIRECTORY libraries DESTINATION bin)
install(DIRECTORY data DESTINATION bin)
install(DIRECTORY internal_data DESTINATION bin)
install(FILES README.md DESTINATION bin)
install(FILES CHANGELOG.md DESTINATION bin)
install(FILES license.txt DESTINATION bin)

include(CPack)