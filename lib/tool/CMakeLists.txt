message("VTX_TOOL")
cmake_minimum_required(VERSION 3.16)
project(VTX_TOOL)

# Git dependencies.
set(GIT_URL_EIGEN https://gitlab.com/libeigen/eigen.git)
set(GIT_TAG_EIGEN 3.4)

# Get CMake helper functions.
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(qt_helper)

# Fill FORMS and QT_RESOURCES file lists.
configure_qt("src")

# Find required packages.
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
if(MSVC)
	find_package(Qt6QWindowsIntegrationPlugin PATHS ${Qt6Gui_DIR}) # Workaround
endif()

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp)

# Fetch third parties.
include(FetchContent)
#FetchContent_Declare(eigen GIT_REPOSITORY ${GIT_URL_EIGEN} GIT_TAG ${GIT_TAG_EIGEN})
#FetchContent_MakeAvailable(eigen)


# Fill FORMS and QT_RESOURCES file lists
find_qt_files()

# Declare library
add_library(VTX_TOOL STATIC ${HEADERS} ${SOURCES} ${FORMS} ${QT_RESOURCES})
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
target_link_libraries(VTX_TOOL PRIVATE VTX_APP)

# Link VTX_UI.
if(NOT TARGET VTX_UI)
	add_subdirectory(../ui lib/ui)
endif()
target_link_libraries(VTX_TOOL PRIVATE VTX_UI)

# Link third parties.
target_link_libraries(VTX_TOOL PRIVATE Qt6::Core)
target_link_libraries(VTX_TOOL PRIVATE Qt6::Gui)
target_link_libraries(VTX_TOOL PRIVATE Qt6::Widgets)

#target_link_libraries(VTX_TOOL PUBLIC eigen)

# Qt options
apply_qt_options(VTX_TOOL)
