file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")

add_executable(vtx_python_binding_test ${SOURCES})
configure_target(vtx_python_binding_test)

if (NOT DEFINED _VTX_PYTHON_BINDING_TEST_CONAN)
	target_link_libraries(vtx_python_binding_test PUBLIC vtx_util)
	target_link_libraries(vtx_python_binding_test PUBLIC vtx_core)
	target_link_libraries(vtx_python_binding_test PUBLIC vtx_io)
	target_link_libraries(vtx_python_binding_test PUBLIC vtx_app)
	target_link_libraries(vtx_python_binding_test PUBLIC vtx_python_binding)
	target_link_libraries(vtx_python_binding_test PUBLIC PyTX)
	target_link_libraries(vtx_python_binding_test PUBLIC Catch2::Catch2WithMain)
else()
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_python_binding::vtx_python_binding)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_python_binding::PyTX)
	target_link_libraries(vtx_python_binding_test PRIVATE Catch2::Catch2WithMain)
endif()

# All other find_package call
vtx_register_build_directory_copy("${CMAKE_CURRENT_LIST_DIR}/../data" "./data")
vtx_copy_registered_data(vtx_python_binding_test)

include(CTest)
include(Catch)
catch_discover_tests(vtx_python_binding_test)