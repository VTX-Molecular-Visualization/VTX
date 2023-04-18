message("VTX_UI")
cmake_minimum_required(VERSION 3.16)
project(VTX_UI)

# Get CMake helper functions.
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(qt_helper)

# Configure Qt
configure_qt()

# Find required packages.
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGL REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
find_package(Qt6 COMPONENTS Network REQUIRED)
find_package(Qt6 COMPONENTS Svg REQUIRED)
find_package(Qt6QGifPlugin PATHS ${Qt6Gui_DIR}) # Workaround
find_package(Qt6QJpegPlugin PATHS ${Qt6Gui_DIR}) # Workaround
if(MSVC)
    find_package(Qt6QWindowsIntegrationPlugin PATHS ${Qt6Gui_DIR}) # Workaround
    find_package(Qt6QSchannelBackendPlugin PATHS ${Qt6Network_DIR}) # Workaround
endif()

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp)

# Get QT forms and resources
file(GLOB_RECURSE QT_FORMS asset/qt/forms/*.ui)
file(GLOB_RECURSE QT_RESOURCES asset/qt/resources/*.qrc)

# Declare library.
add_library(VTX_UI STATIC ${HEADERS} ${SOURCES} ${QT_FORMS} ${QT_RESOURCES})
target_include_directories(VTX_UI PUBLIC include)

# Link VTX_UTIL.
if(NOT TARGET VTX_UTIL)
	add_subdirectory(../util lib/util)
endif()
target_link_libraries(VTX_UI PRIVATE VTX_UTIL)

# Link VTX_APP.
add_subdirectory(../app lib/app)
target_link_libraries(VTX_UI PRIVATE VTX_APP)

# Link third parties.
target_link_libraries(VTX_UI PUBLIC Qt6::Core)
target_link_libraries(VTX_UI PUBLIC Qt6::Gui)
target_link_libraries(VTX_UI PUBLIC Qt6::Widgets)
target_link_libraries(VTX_UI PUBLIC Qt6::OpenGL)
target_link_libraries(VTX_UI PUBLIC Qt6::OpenGLWidgets)
target_link_libraries(VTX_UI PUBLIC Qt6::Network)
target_link_libraries(VTX_UI PUBLIC Qt6::Svg)

# Qt options
apply_qt_options(VTX_UI)
