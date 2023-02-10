message("Project library: VTX_UI")

#Qt
# Create paths.
set(PATH_QT_RESOURCE ${CMAKE_SOURCE_DIR}/asset/qt_resources)
set(VTX_UI_SOURCE_PATH "lib/ui/ui")

# Configure Qt.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "ui/qt/qt_form" )
#set(CMAKE_AUTOGEN_TARGETS_FOLDER "autogen")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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

# Get all QT forms.
file(GLOB_RECURSE FORMS ui/*.ui)
foreach(SRC IN ITEMS ${FORMS})
	# Create group to make visual studio filters.
    source_group("Forms" FILES "${SRC}")
endforeach()

# Get all QT Resources Files.
file(GLOB_RECURSE QT_RESOURCES ${PATH_QT_RESOURCE}/*.qrc)
foreach(SRC IN ITEMS ${QT_RESOURCES})
	# Create group to make visual studio filters.
    source_group("Resources" FILES "${SRC}")
endforeach()

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
find_package(Qt6QWindowsIntegrationPlugin PATHS ${Qt6Gui_DIR}) # Workaround
find_package(Qt6QSchannelBackendPlugin PATHS ${Qt6Network_DIR}) # Workaround

# Third parties.
target_link_libraries(${VTX_UI_LIB} PUBLIC Qt6::Core)
target_link_libraries(${VTX_UI_LIB} PUBLIC Qt6::Gui)
target_link_libraries(${VTX_UI_LIB} PUBLIC Qt6::Widgets)
target_link_libraries(${VTX_UI_LIB} PUBLIC Qt6::OpenGL)
target_link_libraries(${VTX_UI_LIB} PUBLIC Qt6::OpenGLWidgets)
target_link_libraries(${VTX_UI_LIB} PUBLIC Qt6::Network)
target_link_libraries(${VTX_UI_LIB} PUBLIC Qt6::Svg)

# Qt options
set_property(TARGET ${VTX_UI_LIB} PROPERTY AUTOUIC_OPTIONS "--no-autoconnection")

target_include_directories(${VTX_UI_LIB} PUBLIC ui/)
target_include_directories(${VTX_UI_LIB} PUBLIC .)
target_include_directories(${VTX_UI_LIB} PUBLIC ${OLD_VTX_INCLUDE_PATH})
target_include_directories(${VTX_UI_LIB} PUBLIC ${VTX_UTIL_INCLUDE_PATH})

target_link_libraries(${VTX_UI_LIB} PUBLIC ${OLD_VTX_LIB})
target_link_libraries(${VTX_UI_LIB} PUBLIC ${VTX_UTIL_LIB})

# Disable Qt5.
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)