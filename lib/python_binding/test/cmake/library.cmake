file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")

add_executable(vtx_python_binding_test ${SOURCES})
vtx_configure_target(vtx_python_binding_test)

if (NOT DEFINED _VTX_PYTHON_BINDING_TEST_CONAN)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_util)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_core)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_io)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_app)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_python_binding)
	target_link_libraries(vtx_python_binding_test PRIVATE PyTX)	
else()
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_python_binding::vtx_python_binding)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_python_binding::PyTX)
endif()

target_link_libraries(vtx_python_binding_test PRIVATE Catch2::Catch2WithMain)

vtx_copy_directory(vtx_python_binding_test "${CMAKE_CURRENT_LIST_DIR}/../data" "$<TARGET_FILE_DIR:vtx_python_binding_test>/data")

include(CTest)
include(Catch)
catch_discover_tests(vtx_python_binding_test DISCOVERY_MODE PRE_TEST)
