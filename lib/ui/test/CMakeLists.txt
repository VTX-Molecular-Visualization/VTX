message("VTX_UI")
cmake_minimum_required(VERSION 3.20)
include(../util/cmake/configure-target.cmake)
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
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
find_package(Qt6 COMPONENTS Network REQUIRED)
find_package(Qt6 COMPONENTS Svg REQUIRED)
find_package(Qt6QGifPlugin PATHS ${Qt6Gui_DIR}) # Workaround
find_package(Qt6QJpegPlugin PATHS ${Qt6Gui_DIR}) # Workaround
if(WIN32)
    find_package(Qt6QWindowsIntegrationPlugin PATHS ${Qt6Gui_DIR}) # Workaround
    find_package(Qt6QSchannelBackendPlugin PATHS ${Qt6Network_DIR}) # Workaround
endif()

# Get headers and sources.
file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)

# Get QT forms and resources
file(GLOB_RECURSE QT_FORMS asset/qt/forms/*.ui)
file(GLOB_RECURSE QT_RESOURCES asset/qt/resources/*.qrc)

# Declare library.
add_library(VTX_UI STATIC ${HEADERS} ${SOURCES} ${QT_FORMS} ${QT_RESOURCES})
configureTarget(VTX_UI)
target_include_directories(VTX_UI PUBLIC include)

# Link VTX modules.
if(NOT TARGET VTX_UTIL)
	add_subdirectory(../util lib/util)
endif()
if(NOT TARGET VTX_CORE)
    add_subdirectory(../core lib/core)
endif()
if(NOT TARGET VTX_IO)
    add_subdirectory(../io lib/io)
endif()
if(NOT TARGET VTX_APP)
    add_subdirectory(../app lib/app)
endif()
if(NOT TARGET VTX_RENDERER)
    add_subdirectory(../renderer lib/renderer)
endif()
target_link_libraries(VTX_UI PRIVATE VTX_UTIL)
target_link_libraries(VTX_UI PRIVATE VTX_CORE)
target_link_libraries(VTX_UI PRIVATE VTX_IO)
target_link_libraries(VTX_UI PRIVATE VTX_APP)
target_link_libraries(VTX_UI PRIVATE VTX_RENDERER)

# Link third parties.
target_link_libraries(VTX_UI PUBLIC Qt6::Core)
target_link_libraries(VTX_UI PUBLIC Qt6::Gui)
target_link_libraries(VTX_UI PUBLIC Qt6::Widgets)
target_link_libraries(VTX_UI PUBLIC Qt6::OpenGLWidgets)
target_link_libraries(VTX_UI PUBLIC Qt6::Network)
target_link_libraries(VTX_UI PUBLIC Qt6::Svg)

# Qt options
apply_qt_options(VTX_UI)

# Test.
# Create executable.
add_executable(VTX_UI_TEST test/main.cpp)
configureTarget(VTX_UI_TEST)

# Link third parties.
target_link_libraries(VTX_UI_TEST PRIVATE Catch2::Catch2WithMain)

# Link VTX Modules.
target_link_libraries(VTX_UI_TEST PRIVATE VTX_UTIL)
target_link_libraries(VTX_UI_TEST PRIVATE VTX_CORE)
target_link_libraries(VTX_UI_TEST PRIVATE VTX_RENDERER)
target_link_libraries(VTX_UI_TEST PRIVATE VTX_IO)
target_link_libraries(VTX_UI_TEST PRIVATE VTX_APP)
target_link_libraries(VTX_UI_TEST PRIVATE VTX_UI)

# Catch2.
include(CTest)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)	
catch_discover_tests(VTX_UI_TEST)
endif()

# Copy dlls in .exe directory
if(MSVC)
	add_custom_command(TARGET VTX_UI_TEST POST_BUILD
	  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:VTX_UI_TEST> $<TARGET_FILE_DIR:VTX_UI_TEST>
	  COMMAND_EXPAND_LISTS
	)
endif()

# Copy data.
add_custom_target(VTX_UI_TEST_COPY_DATA ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/test/data $<TARGET_FILE_DIR:VTX_UI_TEST>/data)
add_dependencies(VTX_UI_TEST VTX_UI_TEST_COPY_DATA)
