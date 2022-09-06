# Declare CMAKE required version.
cmake_minimum_required(VERSION 3.16)

# Project name.
project(VTX)

# Compiler options.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CONFIGS "Debug;Release") # Windows only.
set(CMAKE_CONFIGURATION_TYPES ${CONFIGS} CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "src/ui/qt_form" "src/view/ui/qt_form" "src/extensions/keyword_editor/forms/")
#set(CMAKE_AUTOGEN_TARGETS_FOLDER "autogen")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Create paths.
set(PATH_SRC "src")
set(PATH_WINDOWS_RESOURCE "asset/windows_resources")
set(PATH_QT_RESOURCE "asset/qt_resources")

# Libs URL and tags.
set(GIT_URL_CHEMFILES https://github.com/sacraiou/chemfiles.git)
set(GIT_TAG_CHEMFILES e1f48fa)
set(GIT_URL_GLM https://github.com/g-truc/glm.git)
set(GIT_TAG_GLM bf71a83) #0.9.9.8
set(GIT_URL_JSON https://github.com/nlohmann/json.git)
set(GIT_TAG_JSON v3.10.5) #3.10.5
set(GIT_URL_MAGIC_ENUM https://github.com/Neargye/magic_enum.git)
set(GIT_TAG_MAGIC_ENUM v0.8.0) #0.8.0

# Create output dir.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}})
	file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

# Copy all sources.
message("Copying all sources...")

# Get all .hpp.
file(GLOB_RECURSE HEADERS ${PATH_SRC}/*.hpp ${PATH_SRC}/*.h)
foreach(SRC IN ITEMS ${HEADERS})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_SRC}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Header${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Get all sources.
file(GLOB_RECURSE SOURCES ${PATH_SRC}/*.cpp)
foreach(SRC IN ITEMS ${SOURCES})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_SRC}" "${SRC_PATH}")
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

# Chemfiles.
FetchContent_Declare(
  chemfiles
  GIT_REPOSITORY ${GIT_URL_CHEMFILES}
  GIT_TAG ${GIT_TAG_CHEMFILES}
)
FetchContent_MakeAvailable(chemfiles)
message("${chemfiles_SOURCE_DIR}")

# Glm.
FetchContent_Declare(
  glm
  GIT_REPOSITORY ${GIT_URL_GLM}
  GIT_TAG ${GIT_TAG_GLM}
)
FetchContent_MakeAvailable(glm)
message("${glm_SOURCE_DIR}")

# Json.
FetchContent_Declare(
  json
  GIT_REPOSITORY ${GIT_URL_JSON}
  GIT_TAG ${GIT_TAG_JSON}
)
FetchContent_MakeAvailable(json)
message("${json_SOURCE_DIR}")

# Magic Enum.
FetchContent_Declare(
  magic_enum
  GIT_REPOSITORY ${GIT_URL_MAGIC_ENUM}
  GIT_TAG ${GIT_TAG_MAGIC_ENUM}
)
FetchContent_MakeAvailable(magic_enum)
message("${magic_enum_SOURCE_DIR}")

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
add_executable(VTX ${HEADERS} ${SOURCES} ${LIBS} ${RESOURCES} ${FORMS} ${QT_RESOURCES})

# Includes directories.
target_include_directories(VTX PRIVATE ${PATH_SRC})

# Link libs.
target_link_libraries(VTX PRIVATE Qt6::Core)
target_link_libraries(VTX PRIVATE Qt6::Gui)
target_link_libraries(VTX PRIVATE Qt6::Widgets)
target_link_libraries(VTX PRIVATE Qt6::OpenGL)
target_link_libraries(VTX PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(VTX PRIVATE Qt6::Network)
target_link_libraries(VTX PRIVATE Qt6::Svg)
target_link_libraries(VTX PRIVATE chemfiles)
target_link_libraries(VTX PRIVATE glm::glm)
target_link_libraries(VTX PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(VTX PRIVATE magic_enum::magic_enum)
target_link_libraries(VTX PRIVATE fmt)

if(MSVC)
	# Add specific libs.
	target_link_libraries(VTX PRIVATE ws2_32)
endif()

message("Creating post build events...")

# Copy shared libs to output (Windows only).	
if(MSVC)
	# Qt for dynamic link.
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Core> $<TARGET_FILE_DIR:VTX>)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Gui> $<TARGET_FILE_DIR:VTX>)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Widgets> $<TARGET_FILE_DIR:VTX>)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::OpenGL> $<TARGET_FILE_DIR:VTX>)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::OpenGLWidgets> $<TARGET_FILE_DIR:VTX>)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Network> $<TARGET_FILE_DIR:VTX>)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::Svg> $<TARGET_FILE_DIR:VTX>)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::QGifPlugin> $<TARGET_FILE_DIR:VTX>/imageformats/$<TARGET_FILE_NAME:Qt6::QGifPlugin>)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::QJpegPlugin> $<TARGET_FILE_DIR:VTX>/imageformats/$<TARGET_FILE_NAME:Qt6::QJpegPlugin>)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::QWindowsIntegrationPlugin> $<TARGET_FILE_DIR:VTX>/platforms/$<TARGET_FILE_NAME:Qt6::QWindowsIntegrationPlugin>)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt6::QSchannelBackendPlugin> $<TARGET_FILE_DIR:VTX>/tls/$<TARGET_FILE_NAME:Qt6::QSchannelBackendPlugin>)
endif()

# Create custom target for GPU sources that copy files to build directory.
add_custom_target(VTX_SHADERS COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/${PATH_SRC}/shader ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../shaders)
set_property(TARGET VTX_SHADERS APPEND PROPERTY SOURCES ${SHADERS}) # Workaround to add source to custom target.

add_dependencies(VTX VTX_SHADERS)

# Copy render effect presets to build directory.
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/libraries/ ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../libraries)

# Copy Data.
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/internal_data ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../data)

# Readme, license and changelog.
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/README.md ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../README.md)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/CHANGELOG.md ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../CHANGELOG.md)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/license.txt ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../license.txt)

# Qt options
set_property(TARGET VTX PROPERTY AUTOUIC_OPTIONS "--no-autoconnection")

# Compiler options #2.
if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    target_compile_options(VTX PRIVATE "-fpermissive")
    target_compile_options(VTX PRIVATE "-w")
    #target_compile_options(VTX PRIVATE "-Wall")
    #target_compile_options(VTX PRIVATE "-Werror")
    #target_compile_options(VTX PRIVATE "-pedantic-errors")
elseif(MSVC)
	# General.
    target_compile_options(VTX PRIVATE "/W3")  # Warning level 3.
	target_compile_options(VTX PRIVATE "/WX")  # Warnings as errors.
	target_compile_options(VTX PRIVATE "/MP")  # Multicore compilation.
	target_compile_options(VTX PRIVATE "/sdl") # SDL check.
	
	target_compile_options(VTX PRIVATE "/utf-8")
	#target_compile_options(VTX PRIVATE "/DEBUG:FASTLINK") # https://devblogs.microsoft.com/cppblog/debugfastlink-for-vs2015-update-1/
	
	# Optimization.
	target_compile_options(VTX PRIVATE "$<$<CONFIG:Release>:/O2>")
	target_compile_options(VTX PRIVATE "$<$<CONFIG:Release>:/Ob2>")
	target_compile_options(VTX PRIVATE "$<$<CONFIG:Release>:/Ot>")
	target_compile_options(VTX PRIVATE "$<$<CONFIG:Release>:/Oi>")
	
	# Code Generation.
	target_compile_options(VTX PRIVATE "/fp:fast") # Floating Point Model.
	
	# Preprocessor definitions
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DNOMINMAX)
		
	# Set default startup project.
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT VTX)
	#set_target_properties(VTX PROPERTIES VS_GLOBAL_DisableFastUpToDateCheck false)
endif()

# Custom options.
option(DEFINE_PRODUCTION "Enable production mode" OFF)
if(DEFINE_PRODUCTION)
	add_definitions(-DVTX_PRODUCTION)
endif()

# Force _DEBUG preprocessor on linux.
if(CMAKE_COMPILER_IS_GNUCC)
	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		add_definitions(-D_DEBUG)
	endif()
endif()

# Wix configuration.
if(WIN32)
	set(CPACK_WIX_UPGRADE_GUID "46ad52f0-a53d-45a1-b170-bfacc26068e2")
endif()

# Disable Qt5.
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)
