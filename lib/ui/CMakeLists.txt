message("Project library: VTX_UI")

#Qt
# Create paths.
set(VTX_UI_SOURCE_PATH "lib/ui/ui")

# Fill FORMS and QT_RESOURCES file lists
configure_qt("ui")

# Get all .hpp.
file(GLOB_RECURSE HEADERS ui/*.hpp ui/*.h)
foreach(SRC IN ITEMS ${HEADERS})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${VTX_UI_SOURCE_PATH}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Header${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Get all sources.
file(GLOB_RECURSE SOURCES ui/*.cpp)
foreach(SRC IN ITEMS ${SOURCES})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${VTX_UI_SOURCE_PATH}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Source${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Fill FORMS and QT_RESOURCES file lists
find_qt_files()

# Declare library
add_library(${VTX_UI_LIB} STATIC ${HEADERS} ${SOURCES} ${FORMS} ${QT_RESOURCES})

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

# Third parties.
target_link_libraries(${VTX_UI_LIB} PUBLIC Qt6::Core)
target_link_libraries(${VTX_UI_LIB} PUBLIC Qt6::Gui)
target_link_libraries(${VTX_UI_LIB} PUBLIC Qt6::Widgets)
target_link_libraries(${VTX_UI_LIB} PUBLIC Qt6::OpenGL)
target_link_libraries(${VTX_UI_LIB} PUBLIC Qt6::OpenGLWidgets)
target_link_libraries(${VTX_UI_LIB} PUBLIC Qt6::Network)
target_link_libraries(${VTX_UI_LIB} PUBLIC Qt6::Svg)

# Qt options
apply_qt_options(${VTX_UI_LIB})

target_include_directories(${VTX_UI_LIB} PUBLIC ui/)
target_include_directories(${VTX_UI_LIB} PUBLIC .)
target_include_directories(${VTX_UI_LIB} PUBLIC ${VTX_APP_INCLUDE_PATH})
target_include_directories(${VTX_UI_LIB} PUBLIC ${VTX_UTIL_INCLUDE_PATH})

target_link_libraries(${VTX_UI_LIB} PUBLIC ${VTX_APP_LIB})
target_link_libraries(${VTX_UI_LIB} PUBLIC ${VTX_UTIL_LIB})
