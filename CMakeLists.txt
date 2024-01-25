message("vtx")
cmake_minimum_required(VERSION 3.21)

project(vtx)

find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(vtx_python_binding CONFIG REQUIRED)
find_package(vtx_renderer CONFIG REQUIRED)
find_package(vtx_tool CONFIG REQUIRED)
find_package(vtx_ui CONFIG REQUIRED)
find_package(vtx_util CONFIG REQUIRED)

file(GLOB_RECURSE HEADERS include/*)
file(GLOB_RECURSE SOURCES src/*)
if(MSVC)
	file(GLOB_RECURSE RESOURCES asset/windows_resources/*.rc)
endif()

add_executable(vtx ${HEADERS} ${SOURCES} ${RESOURCES})
configure_target(vtx)
target_include_directories(vtx PUBLIC include)

target_link_libraries(vtx PRIVATE vtx_app::vtx_app)
target_link_libraries(vtx PRIVATE vtx_core::vtx_core)
target_link_libraries(vtx PRIVATE vtx_io::vtx_io)
target_link_libraries(vtx PRIVATE vtx_python_binding::vtx_python_binding)
target_link_libraries(vtx PRIVATE vtx_renderer::vtx_renderer)
target_link_libraries(vtx PRIVATE vtx_tool::vtx_tool)
target_link_libraries(vtx PRIVATE vtx_ui::vtx_ui)
target_link_libraries(vtx PRIVATE vtx_util::vtx_util)

# TODO: user command line argument?
option(DEFINE_PRODUCTION "Enable production mode" OFF)
if(DEFINE_PRODUCTION)
	add_definitions(-DVTX_PRODUCTION)
endif()

add_custom_target(
	vtx_copy_shader
	ALL COMMAND
	${CMAKE_COMMAND} -E copy_directory	
	${DIR_SHADERS}
	$<TARGET_FILE_DIR:vtx>/shaders)
add_dependencies(vtx vtx_copy_shader)

add_custom_target(
	vtx_copy_python_script
	ALL COMMAND
	${CMAKE_COMMAND} -E copy_directory	
	${DIR_PYTHON_SCRIPT}
	$<TARGET_FILE_DIR:vtx>/python_script)
add_dependencies(vtx vtx_copy_python_script)	

IF (WIN32)
	set(FILENAME_PYTHON_MODULE PyTX.pyd)
ELSE()
	set(FILENAME_PYTHON_MODULE pytx.so)
ENDIF()

add_custom_target(
	vtx_copy_python_module
	ALL COMMAND
	${CMAKE_COMMAND} -E copy
	${PATH_PYTHON_MODULE}
	$<TARGET_FILE_DIR:vtx>/${FILENAME_PYTHON_MODULE})
add_dependencies(vtx vtx_copy_python_module)

install(TARGETS vtx)
install(DIRECTORY ${DIR_SHADERS} DESTINATION bin)
install(DIRECTORY ${DIR_PYTHON_SCRIPT} DESTINATION bin)
install(FILES ${PATH_PYTHON_MODULE} DESTINATION bin RENAME ${FILENAME_PYTHON_MODULE})
install(DIRECTORY libraries DESTINATION bin)
install(DIRECTORY internal_data DESTINATION bin)
install(FILES README.md DESTINATION bin)
install(FILES CHANGELOG.md DESTINATION bin)
install(FILES license.txt DESTINATION bin)

copy_directory_data(vtx $<TARGET_FILE_DIR:vtx>)
