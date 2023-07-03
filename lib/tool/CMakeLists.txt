message("VTX_TOOL")
cmake_minimum_required(VERSION 3.20)
project(VTX_TOOL)

# Git dependencies.
set(GIT_URL_EIGEN https://gitlab.com/libeigen/eigen.git)
set(GIT_TAG_EIGEN 3147391d946bb4b6c68edd901f2add6ac1f31f8c) #3.4.0

# Get CMake helper functions.
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(qt_helper)

# Configure Qt
configure_qt()

# Find required packages.
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
if(MSVC)
	find_package(Qt6QWindowsIntegrationPlugin PATHS ${Qt6Gui_DIR}) # Workaround
endif()

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

# Fetch third parties.
include(FetchContent)
FetchContent_Declare(Eigen3 GIT_REPOSITORY ${GIT_URL_EIGEN} GIT_TAG ${GIT_TAG_EIGEN})
FetchContent_MakeAvailable(Eigen3)

# Get QT forms and resources
file(GLOB_RECURSE QT_FORMS asset/qt/forms/*.ui)
file(GLOB_RECURSE QT_RESOURCES asset/qt/resources/*.qrc)

# Declare library
add_library(VTX_TOOL STATIC ${HEADERS} ${SOURCES} ${QT_FORMS} ${QT_RESOURCES})
target_include_directories(VTX_TOOL PUBLIC include)

# Link VTX modules.
if(NOT TARGET VTX_UTIL)
	add_subdirectory(../util lib/util)
endif()
if(NOT TARGET VTX_APP)
	add_subdirectory(../app lib/app)
endif()
if(NOT TARGET VTX_UI)
	add_subdirectory(../ui lib/ui)
endif()
target_link_libraries(VTX_TOOL PRIVATE VTX_UTIL)
target_link_libraries(VTX_TOOL PRIVATE VTX_APP)
target_link_libraries(VTX_TOOL PRIVATE VTX_UI)

# Link third parties.
target_link_libraries(VTX_TOOL PUBLIC Eigen3::Eigen)
target_link_libraries(VTX_TOOL PRIVATE Qt6::Core)
target_link_libraries(VTX_TOOL PRIVATE Qt6::Gui)
target_link_libraries(VTX_TOOL PRIVATE Qt6::Widgets)

# Qt options
apply_qt_options(VTX_TOOL)
