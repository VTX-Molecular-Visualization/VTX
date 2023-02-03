message("Project library: VTX_TOOL")

# Create paths.
set(VTX_TOOL_SOURCE_PATH "lib/tool/tool")

# Libs URL and tags.
set(GIT_URL_EIGEN https://gitlab.com/libeigen/eigen.git)
set(GIT_TAG_EIGEN 3147391d946bb4b6c68edd901f2add6ac1f31f8c) #3.4.0

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

# Declare library
add_library(VTX_TOOL STATIC ${HEADERS} ${SOURCES} )

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
target_link_libraries(VTX_TOOL PRIVATE Qt6::Core)
target_link_libraries(VTX_TOOL PRIVATE Qt6::Gui)
target_link_libraries(VTX_TOOL PRIVATE Qt6::Widgets)
target_link_libraries(VTX_TOOL PRIVATE Qt6::OpenGL)
target_link_libraries(VTX_TOOL PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(VTX_TOOL PRIVATE Qt6::Network)
target_link_libraries(VTX_TOOL PRIVATE Qt6::Svg)

# Copy shared libs to output (Windows only).	
if(MSVC)
	# Qt for dynamic link.
	add_custom_command(TARGET VTX_TOOL POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Core> $<TARGET_FILE_DIR:VTX_TOOL>)
	add_custom_command(TARGET VTX_TOOL POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Gui> $<TARGET_FILE_DIR:VTX_TOOL>)
	add_custom_command(TARGET VTX_TOOL POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Widgets> $<TARGET_FILE_DIR:VTX_TOOL>)
	add_custom_command(TARGET VTX_TOOL POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::OpenGL> $<TARGET_FILE_DIR:VTX_TOOL>)
	add_custom_command(TARGET VTX_TOOL POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::OpenGLWidgets> $<TARGET_FILE_DIR:VTX_TOOL>)
	add_custom_command(TARGET VTX_TOOL POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Network> $<TARGET_FILE_DIR:VTX_TOOL>)
	add_custom_command(TARGET VTX_TOOL POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Svg> $<TARGET_FILE_DIR:VTX_TOOL>)
	add_custom_command(TARGET VTX_TOOL POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::QGifPlugin> $<TARGET_FILE_DIR:VTX_TOOL>/imageformats/$<TARGET_FILE_NAME:Qt6::QGifPlugin>)
	add_custom_command(TARGET VTX_TOOL POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::QJpegPlugin> $<TARGET_FILE_DIR:VTX_TOOL>/imageformats/$<TARGET_FILE_NAME:Qt6::QJpegPlugin>)
	add_custom_command(TARGET VTX_TOOL POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::QWindowsIntegrationPlugin> $<TARGET_FILE_DIR:VTX_TOOL>/platforms/$<TARGET_FILE_NAME:Qt6::QWindowsIntegrationPlugin>)
	add_custom_command(TARGET VTX_TOOL POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::QSchannelBackendPlugin> $<TARGET_FILE_DIR:VTX_TOOL>/tls/$<TARGET_FILE_NAME:Qt6::QSchannelBackendPlugin>)
endif()

# Qt options
set_property(TARGET VTX_TOOL PROPERTY AUTOUIC_OPTIONS "--no-autoconnection")

target_include_directories(VTX_TOOL PRIVATE tool/)
target_include_directories(VTX_TOOL PRIVATE "${CMAKE_SOURCE_DIR}")

target_link_libraries(VTX_TOOL PRIVATE VTX_UI)
target_link_libraries(VTX_TOOL PRIVATE VTX_UTIL)
#target_link_libraries(VTX_TOOL PRIVATE VTX_APP)

# Disable Qt5.
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)