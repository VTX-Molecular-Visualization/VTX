message("Project library: VTX_TOOL")

# Create paths.
set(VTX_TOOL_SOURCE_PATH "lib/tool/tool")

# Libs URL and tags.
set(GIT_URL_EIGEN https://gitlab.com/libeigen/eigen.git)
set(GIT_TAG_EIGEN 3147391d946bb4b6c68edd901f2add6ac1f31f8c) #3.4.0

configure_qt("tool")

# Get all .hpp.
file(GLOB_RECURSE HEADERS tool/*.hpp tool/*.h)
foreach(SRC IN ITEMS ${HEADERS})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${VTX_TOOL_SOURCE_PATH}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Header${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Get all sources.
file(GLOB_RECURSE SOURCES tool/*.cpp)
foreach(SRC IN ITEMS ${SOURCES})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${VTX_TOOL_SOURCE_PATH}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Source${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Fill FORMS and QT_RESOURCES file lists
find_qt_files()

# Declare library
add_library(${VTX_TOOL_LIB} STATIC ${HEADERS} ${SOURCES} ${FORMS} ${QT_RESOURCES})

# Find required packages.
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

if(MSVC)
	find_package(Qt6QWindowsIntegrationPlugin PATHS ${Qt6Gui_DIR}) # Workaround
endif()

# Third parties.
target_link_libraries(${VTX_TOOL_LIB} PRIVATE Qt6::Core)
target_link_libraries(${VTX_TOOL_LIB} PRIVATE Qt6::Gui)
target_link_libraries(${VTX_TOOL_LIB} PRIVATE Qt6::Widgets)

# Qt options
apply_qt_options(${VTX_TOOL_LIB})

target_include_directories(${VTX_TOOL_LIB} PUBLIC tool/)
target_include_directories(${VTX_TOOL_LIB} PUBLIC .)
target_include_directories(${VTX_TOOL_LIB} PUBLIC ${OLD_VTX_INCLUDE_PATH})
target_include_directories(${VTX_TOOL_LIB} PUBLIC ${VTX_UTIL_INCLUDE_PATH})
target_include_directories(${VTX_TOOL_LIB} PUBLIC ${VTX_UI_INCLUDE_PATH})

target_link_libraries(${VTX_TOOL_LIB} PUBLIC ${OLD_VTX_LIB})
target_link_libraries(${VTX_TOOL_LIB} PUBLIC ${VTX_UTIL_LIB})
target_link_libraries(${VTX_TOOL_LIB} PUBLIC ${VTX_UI_LIB})
