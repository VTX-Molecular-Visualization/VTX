message("vtx_ui_qt")
cmake_minimum_required(VERSION 3.23)
project(vtx_ui_qt)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_python_binding CONFIG REQUIRED)
find_package(Qt6 CONFIG COMPONENTS Core REQUIRED)
find_package(Qt6 CONFIG COMPONENTS Gui REQUIRED)
find_package(Qt6 CONFIG COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS Test REQUIRED)
find_package(EnTT CONFIG REQUIRED) # TODO : see <lib/python_binding/cmake/library.cmake> for details

set(_VTX_UI_QT_CONAN " ") # So the right target name can be linked
include("./cmake/library.cmake")

install(TARGETS vtx_ui_qt FILE_SET public_headers DESTINATION include)
install(DIRECTORY cmake DESTINATION .)


if(WIN32)
    install(FILES
        $<TARGET_FILE_DIR:vtx_ui_qt>/Qt6Core$<$<CONFIG:Debug>:d>.dll
        $<TARGET_FILE_DIR:vtx_ui_qt>/Qt6Gui$<$<CONFIG:Debug>:d>.dll
        $<TARGET_FILE_DIR:vtx_ui_qt>/Qt6Widgets$<$<CONFIG:Debug>:d>.dll
        DESTINATION .
    )
    install(DIRECTORY $<TARGET_FILE_DIR:vtx_ui_qt>/platforms DESTINATION .)
    install(DIRECTORY $<TARGET_FILE_DIR:vtx_ui_qt>/imageformats DESTINATION .)
    install(DIRECTORY $<TARGET_FILE_DIR:vtx_ui_qt>/tls DESTINATION .)
    install(DIRECTORY $<TARGET_FILE_DIR:vtx_ui_qt>/styles DESTINATION .)
endif()
