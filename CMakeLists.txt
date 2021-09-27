# Declare CMAKE required version.
cmake_minimum_required(VERSION 3.13)

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
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
#set(CMAKE_SKIP_RPATH false)
#set(CMAKE_BUILD_RPATH_USE_ORIGIN true)
#set(CMAKE_BUILD_RPATH ".")

# Create paths.
set(PATH_SRC "src")
set(PATH_LIB "lib")
set(PATH_EXTERNAL_PROJECT "external_project")
set(PATH_LIB_BIN "lib_bin")
set(PATH_WINDOWS_RESOURCE "asset/windows_resources")
set(PATH_QT_RESOURCE "asset/qt_resources")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)

# Libs URL and tags.
set(GIT_URL_CHEMFILES https://github.com/sacraiou/chemfiles.git)
set(GIT_TAG_CHEMFILES 6e1493793de7f22bcd5db3858081321cbac0fd3f)
set(GIT_URL_GLM https://github.com/g-truc/glm.git)
set(GIT_TAG_GLM bf71a834948186f4097caa076cd2663c69a10e1e) #0.9.9.8
set(GIT_URL_JSON https://github.com/nlohmann/json.git)
set(GIT_TAG_JSON 626e7d61e44dee32887126c8f437dd077dec09cf) #3.10.2
set(GIT_URL_MAGIC_ENUM https://github.com/Neargye/magic_enum.git)
set(GIT_TAG_MAGIC_ENUM 3d1f6a5a2a3fbcba077e00ad0ccc2dd9fefc2ca7) #0.7.3

# Create output dir.
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
    source_group("Headers${GROUP_PATH}" FILES "${SRC}")
endforeach()

# Get all sources.
file(GLOB_RECURSE SOURCES ${PATH_SRC}/*.cpp)
foreach(SRC IN ITEMS ${SOURCES})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_SOURCE_DIR}/${PATH_SRC}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "/${SRC_PATH_REL}")
	# Create group to make visual studio filters.
    source_group("Sources${GROUP_PATH}" FILES "${SRC}")
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

# Adding required libs.
message("Adding external dependencies...")

include(ExternalProject)

# Glm.
ExternalProject_Add(
    glm
    PREFIX ${CMAKE_SOURCE_DIR}/${PATH_LIB}/g-truc
    GIT_REPOSITORY ${GIT_URL_GLM}
	GIT_TAG ${GIT_TAG_GLM}
	CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    )
ExternalProject_Get_Property(glm SOURCE_DIR)
set(GLM_INCLUDE_DIR ${SOURCE_DIR})

# Json.
ExternalProject_Add(
    json
    PREFIX ${CMAKE_SOURCE_DIR}/${PATH_LIB}/nlohmann
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
    PREFIX ${CMAKE_SOURCE_DIR}/${PATH_LIB}/Neargye
    GIT_REPOSITORY ${GIT_URL_MAGIC_ENUM}
	GIT_TAG ${GIT_TAG_MAGIC_ENUM}
	CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    )
ExternalProject_Get_Property(magic_enum SOURCE_DIR)
set(MAGIC_ENUM_INCLUDE_DIR ${SOURCE_DIR}/include)

# Custom function to build external projects.
function (BuildExternalProject p_url p_tag p_target p_config)

# Write another CMakeLists.txt.
set(CMAKELIST_CONTENT "
cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})
project(build_external_project)
include(ExternalProject)
ExternalProject_add(
	${p_target}
	GIT_REPOSITORY ${p_url}
	GIT_TAG ${p_tag}
	LOG_DOWNLOAD ON
	CMAKE_GENERATOR \"${CMAKE_GENERATOR}\"
	CMAKE_GENERATOR_PLATFORM \"${CMAKE_GENERATOR_PLATFORM}\"
	CMAKE_GENERATOR_TOOLSET \"${CMAKE_GENERATOR_TOOLSET}\"
	CMAKE_GENERATOR_INSTANCE \"${CMAKE_GENERATOR_INSTANCE}\"
	CMAKE_ARGS -D CMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/${PATH_LIB}/${p_target}
)
add_custom_target(build_external_project)
add_dependencies(build_external_project ${p_target})
")

	set(TARGET_DIR "${CMAKE_SOURCE_DIR}/${PATH_EXTERNAL_PROJECT}/${p_target}")
	file(WRITE "${TARGET_DIR}/CMakeLists.txt" "${CMAKELIST_CONTENT}")
	file(MAKE_DIRECTORY "${TARGET_DIR}" "${TARGET_DIR}/build")
	
	# Run lib cmake.
	execute_process(COMMAND ${CMAKE_COMMAND}
		-G "${CMAKE_GENERATOR}"
		-A "${CMAKE_GENERATOR_PLATFORM}"
		-T "${CMAKE_GENERATOR_TOOLSET}"
		-DCMAKE_BUILD_TYPE=${p_config}
		..
		WORKING_DIRECTORY "${TARGET_DIR}/build")

	# Build lib.
	execute_process(COMMAND ${CMAKE_COMMAND}
		--build .
		--config ${p_config}
		WORKING_DIRECTORY "${TARGET_DIR}/build")

endfunction()

message("Building external projects...")

option(BUILD_LIB "Build libs" ON)
if(BUILD_LIB)
	# Chemfiles.
	# For windows(multi conf), build in Debug then in Release, and move .lib after cmake install to the correct folder.
	# Then, use link_directories to link and and target_include_directories to add include directory.
	if(MSVC)
		BuildExternalProject(${GIT_URL_CHEMFILES} ${GIT_TAG_CHEMFILES} chemfiles Debug)
		file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/${PATH_LIB}/chemfiles/lib/Debug/ ${CMAKE_SOURCE_DIR}/${PATH_LIB}/chemfiles/lib/Release/)
		file(COPY ${CMAKE_SOURCE_DIR}/${PATH_LIB}/chemfiles/lib/chemfiles.lib DESTINATION ${CMAKE_SOURCE_DIR}/${PATH_LIB}/chemfiles/lib/Debug/)
		BuildExternalProject(${GIT_URL_CHEMFILES} ${GIT_TAG_CHEMFILES} chemfiles Release)
		file(COPY ${CMAKE_SOURCE_DIR}/${PATH_LIB}/chemfiles/lib/chemfiles.lib DESTINATION ${CMAKE_SOURCE_DIR}/${PATH_LIB}/chemfiles/lib/Release/)
		file(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/${PATH_LIB}/chemfiles/lib/cmake)
		file(REMOVE ${CMAKE_SOURCE_DIR}/${PATH_LIB}/chemfiles/lib/chemfiles.lib)
	elseif(CMAKE_COMPILER_IS_GNUCC)
	# For Linux (solo conf), juste build in current mode.
	# Then use find_package to automatically add include folder and link lib. 
		BuildExternalProject(${GIT_URL_CHEMFILES} ${GIT_TAG_CHEMFILES} chemfiles "${CMAKE_BUILD_TYPE}")
	endif()
endif()

message("Looking for dependencies...")

# Set link directories.
if(MSVC)
	link_directories(${PATH_LIB}/chemfiles/lib)
elseif(CMAKE_COMPILER_IS_GNUCC)
	find_package(chemfiles REQUIRED PATHS "${CMAKE_SOURCE_DIR}/lib/chemfiles" NO_DEFAULT_PATH)
endif()

# Find required packages.
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
#find_package(Qt5 COMPONENTS Gamepad REQUIRED) 
find_package(Qt5 COMPONENTS Svg REQUIRED)

# Declare executable with source files.
add_executable(VTX ${HEADERS} ${SOURCES} ${LIBS} ${RESOURCES} ${FORMS} ${QT_RESOURCES})

# Make project waiting for dependencies download/build.
add_dependencies(VTX glm)
add_dependencies(VTX json)
add_dependencies(VTX magic_enum)

# Includes directories.
target_include_directories(VTX PRIVATE ${PATH_SRC})
target_include_directories(VTX PRIVATE ${GLM_INCLUDE_DIR})
target_include_directories(VTX PRIVATE ${JSON_INCLUDE_DIR})
target_include_directories(VTX PRIVATE ${MAGIC_ENUM_INCLUDE_DIR})
if(MSVC)
	target_include_directories(VTX PRIVATE ${PATH_LIB}/chemfiles/include)
endif()

# Link libs.
target_link_libraries(VTX PRIVATE Qt5::Core)
target_link_libraries(VTX PRIVATE Qt5::Gui)
target_link_libraries(VTX PRIVATE Qt5::Widgets)
target_link_libraries(VTX PRIVATE Qt5::Network)
#target_link_libraries(VTX PRIVATE Qt5::Gamepad)
target_link_libraries(VTX PRIVATE Qt5::Svg)
target_link_libraries(VTX PRIVATE chemfiles)

if(MSVC)
	# Add specific libs.
	target_link_libraries(VTX PRIVATE ws2_32)
elseif(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_EXE_LINKER_FLAGS " -static")
	target_link_libraries(VTX -static-libgcc -static-libstdc++)
endif()

message("Creating post build events...")

# Copy shared libs to output (Windows only).	
if(MSVC)
	# Debug specific libs.
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB_BIN}/libcrypto-1_1-x64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/libcrypto-1_1-x64.dll)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB_BIN}/libssl-1_1-x64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/libssl-1_1-x64.dll)
	# Release specific libs.
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB_BIN}/libcrypto-1_1-x64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/libcrypto-1_1-x64.dll)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${PATH_LIB_BIN}/libssl-1_1-x64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/libssl-1_1-x64.dll)
	
	# Qt for dynamic link.
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Core> $<TARGET_FILE_DIR:VTX>)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Gui> $<TARGET_FILE_DIR:VTX>)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Widgets> $<TARGET_FILE_DIR:VTX>)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Network> $<TARGET_FILE_DIR:VTX>)
	#add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Gamepad> $<TARGET_FILE_DIR:VTX>)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Svg> $<TARGET_FILE_DIR:VTX>)
	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::QGifPlugin> $<TARGET_FILE_DIR:VTX>/imageformats/$<TARGET_FILE_NAME:Qt5::QGifPlugin>)

	add_custom_command(TARGET VTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::QWindowsIntegrationPlugin> $<TARGET_FILE_DIR:VTX>/platforms/$<TARGET_FILE_NAME:Qt5::QWindowsIntegrationPlugin>)
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
	target_compile_options(VTX PRIVATE "/execution-charset:utf-8") # To display Angstrom symbol for example.
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