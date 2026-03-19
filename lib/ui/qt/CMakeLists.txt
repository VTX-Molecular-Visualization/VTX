message("vtx_ui_qt")
cmake_minimum_required(VERSION 3.23)
project(vtx_ui_qt)

find_package(vtx_util CONFIG REQUIRED)
set (PYBIND11_PYTHON_VERSION "${CPYTHON_VERSION_MAJOR}.${CPYTHON_VERSION_MINOR}.${CPYTHON_VERSION_PATCH}")
find_package(Python3 "${CPYTHON_VERSION_MAJOR}.${CPYTHON_VERSION_MINOR}.${CPYTHON_VERSION_PATCH}" EXACT REQUIRED COMPONENTS Interpreter Development NO_SYSTEM_ENVIRONMENT_PATH)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_python_binding CONFIG REQUIRED)
find_package(Qt6 CONFIG COMPONENTS Core REQUIRED)
find_package(Qt6 CONFIG COMPONENTS Gui REQUIRED)
find_package(Qt6 CONFIG COMPONENTS Widgets REQUIRED)

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
elseif(UNIX AND NOT APPLE)
    install(DIRECTORY $<TARGET_FILE_DIR:vtx_ui_qt>/ DESTINATION .
        FILES_MATCHING
        PATTERN "libQt6*.so*"
    )
    install(DIRECTORY $<TARGET_FILE_DIR:vtx_ui_qt>/platforms DESTINATION . OPTIONAL)
    install(DIRECTORY $<TARGET_FILE_DIR:vtx_ui_qt>/imageformats DESTINATION . OPTIONAL)
    install(DIRECTORY $<TARGET_FILE_DIR:vtx_ui_qt>/tls DESTINATION . OPTIONAL)
    install(DIRECTORY $<TARGET_FILE_DIR:vtx_ui_qt>/styles DESTINATION . OPTIONAL)
    install(DIRECTORY $<TARGET_FILE_DIR:vtx_ui_qt>/platformthemes DESTINATION . OPTIONAL)
    install(DIRECTORY $<TARGET_FILE_DIR:vtx_ui_qt>/xcbglintegrations DESTINATION . OPTIONAL)
    install(DIRECTORY $<TARGET_FILE_DIR:vtx_ui_qt>/egldeviceintegrations DESTINATION . OPTIONAL)
    install(DIRECTORY $<TARGET_FILE_DIR:vtx_ui_qt>/wayland-decoration-client DESTINATION . OPTIONAL)
    install(DIRECTORY $<TARGET_FILE_DIR:vtx_ui_qt>/wayland-graphics-integration-client DESTINATION . OPTIONAL)
    install(DIRECTORY $<TARGET_FILE_DIR:vtx_ui_qt>/wayland-shell-integration DESTINATION . OPTIONAL)
endif()
