message("vtx_python_binding_test")
cmake_minimum_required(VERSION 3.23)
include(cmake/configure-target.cmake)
project(vtx_python_binding_test)

find_package(vtx_util CONFIG REQUIRED)
find_package(vtx_io CONFIG REQUIRED)
find_package(vtx_core CONFIG REQUIRED)
find_package(vtx_app CONFIG REQUIRED)
find_package(vtx_python_binding CONFIG REQUIRED COMPONENTS vtx_python_binding::vtx_python_binding vtx_python_binding::PyTX)
find_package(pybind11 REQUIRED)
find_package(Catch2 REQUIRED)

file(GLOB_RECURSE SOURCES src/*)
add_executable(vtx_python_binding_test ${SOURCES})
configureTarget(vtx_python_binding_test)

target_link_libraries(vtx_python_binding_test PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_python_binding_test PRIVATE vtx_io::vtx_io)
target_link_libraries(vtx_python_binding_test PRIVATE vtx_core::vtx_core)
target_link_libraries(vtx_python_binding_test PRIVATE vtx_app::vtx_app)
target_link_libraries(vtx_python_binding_test PRIVATE vtx_python_binding::vtx_python_binding)
target_link_libraries(vtx_python_binding_test PRIVATE vtx_python_binding::PyTX)
target_link_libraries(vtx_python_binding_test PRIVATE pybind11::embed)
target_link_libraries(vtx_python_binding_test PRIVATE Catch2::Catch2WithMain)

add_custom_target(vtx_python_binding_copy_script ALL COMMAND
	${CMAKE_COMMAND} -E copy_directory	
	${DIR_PYTHON_SCRIPT}
	$<TARGET_FILE_DIR:vtx_python_binding_test>/python_script)
add_dependencies(vtx_python_binding_test vtx_python_binding_copy_script)

add_custom_target(vtx_python_binding_copy_module ALL COMMAND
	${CMAKE_COMMAND} -E copy
	${PATH_PYTHON_MODULE}
	$<TARGET_FILE_DIR:vtx_python_binding_test>/PyTX.pyd)
add_dependencies(vtx_python_binding_test vtx_python_binding_copy_module)

add_custom_target(vtx_python_binding_test_copy_data ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:vtx_python_binding_test>/data)
add_dependencies(vtx_python_binding_test vtx_python_binding_test_copy_data)

include(CTest)
include(Catch)
catch_discover_tests(vtx_python_binding_test)

install(TARGETS vtx_python_binding_test)
install(DIRECTORY ${DIR_PYTHON_SCRIPT} DESTINATION bin)
install(FILES ${PATH_PYTHON_MODULE} DESTINATION bin RENAME PyTX.pyd)
install(DIRECTORY data DESTINATION bin)