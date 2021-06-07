# Declare CMAKE required version.
cmake_minimum_required(VERSION 3.13)

# Project name.
project(VTX)

# Create paths.
set(PATH_SRC "src")
set(PATH_LIB "lib")
set(PATH_LIB_BIN "lib_bin")
set(PATH_WINDOWS_RESOURCE "_windows/resource")
set(PATH_QT_RESOURCE "asset/qt_resources")
set(CONFIGS "Debug;Release;RelWithDebInfo")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)

if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}})
	file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

message("Copying all sources...")

# Get all .hpp.
file(GLOB_RECURSE HEADERS ${PATH_SRC}/*.hpp ${PATH_SRC}/*.h)
foreach(SRC IN ITEMS ${HEADERS})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_SRC}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Headers${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Get all sources.
file(GLOB_RECURSE SOURCES ${PATH_SRC}/*.cpp ${PATH_SRC}/*.cu)
foreach(SRC IN ITEMS ${SOURCES})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_SRC}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Sources${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Get all resources.
file(GLOB_RECURSE RESOURCES ${PATH_WINDOWS_RESOURCE}/*.rc)
foreach(SRC IN ITEMS ${RESOURCES})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_WINDOWS_RESOURCE}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Resources${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Get all GLSL sources.
file(GLOB_RECURSE SHADERS ${PATH_SRC}/*.vert ${PATH_SRC}/*.frag ${PATH_SRC}/*.geom ${PATH_SRC}/*.tesc ${PATH_SRC}/*.tese)
foreach(SRC IN ITEMS ${SHADERS})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_SRC}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Sources" FILES "${SRC}")
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
    source_group("QtResources" FILES "${SRC}")
endforeach()

message("Copying all sources... DONE")

# Compiler options #1.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CONFIGURATION_TYPES ${CONFIGS} CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "src/ui/qt_form" "src/view/ui/qt_form" "src/extensions/keyword_editor/forms/")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set link directories.
link_directories(${PATH_LIB}/assimp/lib/x64)
link_directories(${PATH_LIB}/chemfiles/lib/x64)

# Product properties.
#add_definitions(-DVER_COMPANYNAME_STR="LALALALAL")
#add_definitions(-DVER_FILEVERSION_STR="6,1,0.0")
	
# Declare executable with source files.
add_executable(VTX ${HEADERS} ${SOURCES} ${LIBS} ${RESOURCES} ${FORMS} ${QT_RESOURCES})

# Find required packages.
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS Gamepad REQUIRED) 
find_package(Qt5 COMPONENTS Svg REQUIRED)

# Includes directories.
target_include_directories(VTX PRIVATE ${PATH_SRC})
target_include_directories(VTX PRIVATE ${PATH_LIB})
target_include_directories(VTX PRIVATE ${PATH_LIB}/msgpack)
target_include_directories(VTX PRIVATE ${PATH_LIB}/assimp/include)
target_include_directories(VTX PRIVATE ${PATH_LIB}/chemfiles/include)

# Link libs.
target_link_libraries(VTX PRIVATE assimp-vc142-mt)
target_link_libraries(VTX PRIVATE IrrXML)
target_link_libraries(VTX PRIVATE zlib)
target_link_libraries(VTX PRIVATE zlibstatic)
target_link_libraries(VTX PRIVATE chemfiles)
target_link_libraries(VTX PRIVATE Qt5::Core)
target_link_libraries(VTX PRIVATE Qt5::Gui)
target_link_libraries(VTX PRIVATE Qt5::Widgets)
target_link_libraries(VTX PRIVATE Qt5::Network)
target_link_libraries(VTX PRIVATE Qt5::Gamepad)
target_link_libraries(VTX PRIVATE Qt5::Svg)

if(WIN32)
	# Add specific libs.
	target_link_libraries(VTX PRIVATE ws2_32)
 endif()
 
# Copy DLL to output.	
# Debug
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/assimp/bin/x64/Debug/assimp-vc142-mtd.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/assimp-vc142-mtd.dll)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/assimp/bin/x64/Debug/zlibd.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/zlibd1.dll)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB_BIN}/libcrypto-1_1-x64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/libcrypto-1_1-x64.dll)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB_BIN}/libssl-1_1-x64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/libssl-1_1-x64.dll)
# Release
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/assimp/bin/x64/Release/assimp-vc142-mt.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/assimp-vc142-mt.dll)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/assimp/bin/x64/Release/zlib.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/zlib1.dll)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB_BIN}/libcrypto-1_1-x64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/libcrypto-1_1-x64.dll)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB_BIN}/libssl-1_1-x64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/libssl-1_1-x64.dll)
# RelWithDebInfo
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/assimp/bin/x64/RelWithDebInfo/assimp-vc142-mt.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/assimp-vc142-mt.dll)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB}/assimp/bin/x64/RelWithDebInfo/zlib.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/zlib1.dll)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB_BIN}/libcrypto-1_1-x64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/libcrypto-1_1-x64.dll)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB_BIN}/libssl-1_1-x64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/libssl-1_1-x64.dll)

# Copy Qt DLL to output.
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Core> $<TARGET_FILE_DIR:VTX>)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Gui> $<TARGET_FILE_DIR:VTX>)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Widgets> $<TARGET_FILE_DIR:VTX>)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Network> $<TARGET_FILE_DIR:VTX>)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Gamepad> $<TARGET_FILE_DIR:VTX>)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Svg> $<TARGET_FILE_DIR:VTX>)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::QWindowsIntegrationPlugin> $<TARGET_FILE_DIR:VTX>/platforms/$<TARGET_FILE_NAME:Qt5::QWindowsIntegrationPlugin>)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::QGifPlugin> $<TARGET_FILE_DIR:VTX>/imageformats/$<TARGET_FILE_NAME:Qt5::QGifPlugin>)

# Create custom target for GPU sources that copy files to build direcctory.
add_custom_target(VTX_SHADERS COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/${PATH_SRC}/shader ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../shaders)
set_property(TARGET VTX_SHADERS APPEND PROPERTY SOURCES ${SHADERS}) # Workaround to add source to custom target.

add_dependencies(VTX VTX_SHADERS)

# Readme, license and changelog.
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/README.md ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../README.md)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/LICENSE.md ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../LICENSE.md)
add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/license.txt ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../license.txt)

# Qt options
set_property(TARGET VTX PROPERTY AUTOUIC_OPTIONS "--no-autoconnection")

# Compiler options #2.
if(CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(VTX PRIVATE "-Wall -Wextra")
elseif(MSVC)
	# General.
    target_compile_options(VTX PRIVATE "/W3")  # Warning level 3.
	target_compile_options(VTX PRIVATE "/WX")  # Warnings as errors.
	target_compile_options(VTX PRIVATE "/MP")  # Multicore compilation.
	target_compile_options(VTX PRIVATE "/sdl") # SDL check.
	target_compile_options(VTX PRIVATE "/execution-charset:utf-8") # To display Angstrom symbol for example.
	#target_compile_options(VTX PRIVATE "/DEBUG:FASTLINK") # https://devblogs.microsoft.com/cppblog/debugfastlink-for-vs2015-update-1/
	
	# Optimization.
	target_compile_options(VTX PRIVATE "$<$<CONFIG:Release>:/O2>")
	target_compile_options(VTX PRIVATE "$<$<CONFIG:Release>:/Ob2>")
	target_compile_options(VTX PRIVATE "$<$<CONFIG:Release>:/Ot>")
	target_compile_options(VTX PRIVATE "$<$<CONFIG:Release>:/Oi>")
	target_compile_options(VTX PRIVATE "$<$<CONFIG:RelWithDebInfo>:/O2>")
	target_compile_options(VTX PRIVATE "$<$<CONFIG:RelWithDebInfo>:/Ob2>")
	target_compile_options(VTX PRIVATE "$<$<CONFIG:RelWithDebInfo>:/Ot>")
	target_compile_options(VTX PRIVATE "$<$<CONFIG:RelWithDebInfo>:/Oi>")
	
	# Code Generation.
	target_compile_options(VTX PRIVATE "/fp:fast") # Floating Point Model.
	
	# Preprocessor definitions
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DNOMINMAX)
	
	# Set default startup project.
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT VTX)
	#set_target_properties(VTX PROPERTIES VS_GLOBAL_DisableFastUpToDateCheck false)
endif()