message("Project library: OLD_VTX")

# Create paths.
set(PATH_SRC "old")
set(OLD_VTX_SOURCE_PATH "lib/old/old")
set(PATH_WINDOWS_RESOURCE "asset/windows_resources")
set(PATH_QT_RESOURCE "asset/qt_resources")


# Configure Qt.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "old/ui/qt_form" "old/view/ui/qt_form")
#set(CMAKE_AUTOGEN_TARGETS_FOLDER "autogen")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Libs URL and tags.
set(GIT_URL_CHEMFILES https://github.com/sacraiou/chemfiles.git)
set(GIT_TAG_CHEMFILES e6177423c015ac6ff1a0307b3c3a67c7850d0fe2)
set(GIT_URL_JSON https://github.com/nlohmann/json.git)
set(GIT_TAG_JSON v3.10.5) #3.10.5
set(GIT_URL_MAGIC_ENUM https://github.com/Neargye/magic_enum.git)
set(GIT_TAG_MAGIC_ENUM v0.8.0) #0.8.0
set(GIT_URL_EIGEN https://gitlab.com/libeigen/eigen.git)
set(GIT_TAG_EIGEN 3147391d946bb4b6c68edd901f2add6ac1f31f8c) #3.4.0

# Copy all sources.
message("Copying all sources...")

# Get all .hpp.
file(GLOB_RECURSE HEADERS ${PATH_SRC}/*.hpp ${PATH_SRC}/*.h)
foreach(SRC IN ITEMS ${HEADERS})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${OLD_VTX_SOURCE_PATH}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Header${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Get all sources.
file(GLOB_RECURSE SOURCES ${PATH_SRC}/*.cpp)
foreach(SRC IN ITEMS ${SOURCES})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${OLD_VTX_SOURCE_PATH}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Source${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Get all resources.
if(MSVC)
	file(GLOB_RECURSE RESOURCES ${PATH_WINDOWS_RESOURCE}/*.rc)
	foreach(SRC IN ITEMS ${RESOURCES})
		get_filename_component(SRC_PATH "${SRC}" PATH)
		file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_WINDOWS_RESOURCE}" "${SRC_PATH}")
		string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
		# Create group to make visual studio filters.
		source_group("Resources${GROUP_PATH}" FILES "${SRC}")
	endforeach()
endif()

# Get all GLSL sources.
file(GLOB_RECURSE SHADERS ${PATH_SRC}/*.vert ${PATH_SRC}/*.frag ${PATH_SRC}/*.geom ${PATH_SRC}/*.tesc ${PATH_SRC}/*.tese ${PATH_SRC}/*.comp)
foreach(SRC IN ITEMS ${SHADERS})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_SRC}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Source" FILES "${SRC}")
endforeach()

# Get all QT forms.
file(GLOB_RECURSE FORMS ${PATH_SRC}/*.ui)
foreach(SRC IN ITEMS ${FORMS})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_SRC}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Forms" FILES "${SRC}")
endforeach()

# Get all QT Resources Files.
file(GLOB_RECURSE QT_RESOURCES ${PATH_QT_RESOURCE}/*.qrc)
foreach(SRC IN ITEMS ${QT_RESOURCES})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_QT_RESOURCE}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Resources" FILES "${SRC}")
endforeach()


# Download dependencies.
message("Downloading dependencies...")

include(FetchContent)
include(ExternalProject)

# Chemfiles.
FetchContent_Declare(
  chemfiles
  GIT_REPOSITORY ${GIT_URL_CHEMFILES}
  GIT_TAG ${GIT_TAG_CHEMFILES}
)
FetchContent_MakeAvailable(chemfiles)

# Json.
ExternalProject_Add(
    json
    PREFIX ${CMAKE_SOURCE_DIR}/${PATH_EXTERNAL_PROJECT}/nlohmann
    GIT_REPOSITORY ${GIT_URL_JSON}
	GIT_TAG ${GIT_TAG_JSON}
	CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    )
ExternalProject_Get_Property(json SOURCE_DIR)
set(JSON_INCLUDE_DIR ${SOURCE_DIR}/include/)

# Magic Enum.
ExternalProject_Add(
    magic_enum
    PREFIX ${CMAKE_SOURCE_DIR}/${PATH_EXTERNAL_PROJECT}/Neargye
    GIT_REPOSITORY ${GIT_URL_MAGIC_ENUM}
	GIT_TAG ${GIT_TAG_MAGIC_ENUM}
	CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    )
ExternalProject_Get_Property(magic_enum SOURCE_DIR)
set(MAGIC_ENUM_INCLUDE_DIR ${SOURCE_DIR}/include)

# Eigen.
ExternalProject_Add(
    eigen
    PREFIX ${CMAKE_SOURCE_DIR}/${PATH_EXTERNAL_PROJECT}/Eigen
    GIT_REPOSITORY ${GIT_URL_EIGEN}
	GIT_TAG ${GIT_TAG_EIGEN}
	CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    )
ExternalProject_Get_Property(eigen SOURCE_DIR)
set(EIGEN_INCLUDE_DIR ${SOURCE_DIR}/)

message("Looking for dependencies...")

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

# Declare executable with source files.
add_library(${OLD_VTX_LIB} STATIC ${HEADERS} ${SOURCES} ${RESOURCES} ${FORMS} ${QT_RESOURCES})

# Includes directories.
target_include_directories(${OLD_VTX_LIB} PRIVATE ${PATH_SRC})
target_include_directories(${OLD_VTX_LIB} PUBLIC ${JSON_INCLUDE_DIR})
target_include_directories(${OLD_VTX_LIB} PUBLIC ${MAGIC_ENUM_INCLUDE_DIR})
target_include_directories(${OLD_VTX_LIB} PUBLIC ${EIGEN_INCLUDE_DIR})

# Link modules.
# Custom.
target_include_directories(${OLD_VTX_LIB} PRIVATE ${VTX_UTIL_INCLUDE_PATH})
target_link_libraries(${OLD_VTX_LIB} PUBLIC ${VTX_UTIL_LIB})

# Third parties.
target_link_libraries(${OLD_VTX_LIB} PUBLIC Qt6::Core)
target_link_libraries(${OLD_VTX_LIB} PUBLIC Qt6::Gui)
target_link_libraries(${OLD_VTX_LIB} PUBLIC Qt6::Widgets)
target_link_libraries(${OLD_VTX_LIB} PUBLIC Qt6::OpenGL)
target_link_libraries(${OLD_VTX_LIB} PUBLIC Qt6::OpenGLWidgets)
target_link_libraries(${OLD_VTX_LIB} PUBLIC Qt6::Network)
target_link_libraries(${OLD_VTX_LIB} PUBLIC Qt6::Svg)
target_link_libraries(${OLD_VTX_LIB} PUBLIC chemfiles)
target_link_libraries(${OLD_VTX_LIB} PUBLIC fmt)

if(MSVC)
	# Add specific libs.
	target_link_libraries(${OLD_VTX_LIB} PRIVATE ws2_32)
endif()

message("Creating post build events...")

# Copy shared libs to output (Windows only).	
if(MSVC)
	# Qt for dynamic link.
	add_custom_command(TARGET ${OLD_VTX_LIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Core> $<TARGET_FILE_DIR:${OLD_VTX_LIB}>)
	add_custom_command(TARGET ${OLD_VTX_LIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Gui> $<TARGET_FILE_DIR:${OLD_VTX_LIB}>)
	add_custom_command(TARGET ${OLD_VTX_LIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Widgets> $<TARGET_FILE_DIR:${OLD_VTX_LIB}>)
	add_custom_command(TARGET ${OLD_VTX_LIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::OpenGL> $<TARGET_FILE_DIR:${OLD_VTX_LIB}>)
	add_custom_command(TARGET ${OLD_VTX_LIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::OpenGLWidgets> $<TARGET_FILE_DIR:${OLD_VTX_LIB}>)
	add_custom_command(TARGET ${OLD_VTX_LIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Network> $<TARGET_FILE_DIR:${OLD_VTX_LIB}>)
	add_custom_command(TARGET ${OLD_VTX_LIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Svg> $<TARGET_FILE_DIR:${OLD_VTX_LIB}>)
	add_custom_command(TARGET ${OLD_VTX_LIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::QGifPlugin> $<TARGET_FILE_DIR:${OLD_VTX_LIB}>/imageformats/$<TARGET_FILE_NAME:Qt6::QGifPlugin>)
	add_custom_command(TARGET ${OLD_VTX_LIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::QJpegPlugin> $<TARGET_FILE_DIR:${OLD_VTX_LIB}>/imageformats/$<TARGET_FILE_NAME:Qt6::QJpegPlugin>)
	add_custom_command(TARGET ${OLD_VTX_LIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::QWindowsIntegrationPlugin> $<TARGET_FILE_DIR:${OLD_VTX_LIB}>/platforms/$<TARGET_FILE_NAME:Qt6::QWindowsIntegrationPlugin>)
	add_custom_command(TARGET ${OLD_VTX_LIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::QSchannelBackendPlugin> $<TARGET_FILE_DIR:${OLD_VTX_LIB}>/tls/$<TARGET_FILE_NAME:Qt6::QSchannelBackendPlugin>)
endif()

# Create custom target for GPU sources that copy files to build directory.
add_custom_target(VTX_SHADERS COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/lib/old/old/shader ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../shaders)
set_property(TARGET VTX_SHADERS APPEND PROPERTY SOURCES ${SHADERS}) # Workaround to add source to custom target.

add_dependencies(${OLD_VTX_LIB} VTX_SHADERS)

# Qt options
set_property(TARGET ${OLD_VTX_LIB} PROPERTY AUTOUIC_OPTIONS "--no-autoconnection")

# Disable Qt5.
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)
