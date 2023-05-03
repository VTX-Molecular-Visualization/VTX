message("VTX_TOOL")
cmake_minimum_required(VERSION 3.16)
project(VTX_TOOL)

# Git dependencies.
set(GIT_URL_EIGEN https://gitlab.com/libeigen/eigen.git)
# set(GIT_TAG_EIGEN 3.4)
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
# Eigen.

# This part doesn't works because of a target name conflict between Eigen and chemfiles ("uninstall" target in each lib)
#include(FetchContent)
#FetchContent_Declare(eigen GIT_REPOSITORY ${GIT_URL_EIGEN} GIT_TAG ${GIT_TAG_EIGEN})
#FetchContent_MakeAvailable(eigen)

# TODO Find a better way to include Eigen
ExternalProject_Add(
    eigen
    PREFIX "${CMAKE_SOURCE_DIR}/external_project/Eigen"
    GIT_REPOSITORY ${GIT_URL_EIGEN}
	GIT_TAG ${GIT_TAG_EIGEN}
	CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    )
ExternalProject_Get_Property(eigen SOURCE_DIR)
set(EIGEN_INCLUDE_DIR ${SOURCE_DIR}/)

# Get QT forms and resources
file(GLOB_RECURSE QT_FORMS asset/qt/forms/*.ui)
file(GLOB_RECURSE QT_RESOURCES asset/qt/resources/*.qrc)

# Declare library
add_library(VTX_TOOL STATIC ${HEADERS} ${SOURCES} ${QT_FORMS} ${QT_RESOURCES})
target_include_directories(VTX_TOOL PUBLIC include)

# Link VTX_UTIL.
if(NOT TARGET VTX_UTIL)
	add_subdirectory(../util lib/util)
endif()
target_link_libraries(VTX_TOOL PRIVATE VTX_UTIL)

# Link VTX_APP.
if(NOT TARGET VTX_APP)
	add_subdirectory(../app lib/app)
endif()
target_link_libraries(VTX_TOOL PUBLIC VTX_APP)

# Link VTX_UI.
if(NOT TARGET VTX_UI)
	add_subdirectory(../ui lib/ui)
endif()
target_link_libraries(VTX_TOOL PUBLIC VTX_UI)

# Link third parties.
target_link_libraries(VTX_TOOL PRIVATE Qt6::Core)
target_link_libraries(VTX_TOOL PRIVATE Qt6::Gui)
target_link_libraries(VTX_TOOL PRIVATE Qt6::Widgets)

#target_link_libraries(VTX_TOOL PUBLIC eigen)
target_include_directories(VTX_TOOL PUBLIC ${EIGEN_INCLUDE_DIR})

# Qt options
apply_qt_options(VTX_TOOL)
