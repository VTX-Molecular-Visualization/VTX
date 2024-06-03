include("${CMAKE_CURRENT_LIST_DIR}/copy_files.cmake")

add_executable(vtx_io_test "${CMAKE_CURRENT_LIST_DIR}/../src/main.cpp")
configure_target(vtx_io_test)

if(NOT DEFINED _VTX_IO_TEST_CONAN)
	target_link_libraries(vtx_io_test PRIVATE vtx_util)
	target_link_libraries(vtx_io_test PRIVATE vtx_core)
	target_link_libraries(vtx_io_test PRIVATE vtx_io)
	target_link_libraries(vtx_io_test PRIVATE Catch2::Catch2WithMain)

else()
	target_link_libraries(vtx_io_test PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_io_test PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_io_test PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_io_test PRIVATE Catch2::Catch2WithMain)
endif()

vtx_copy_registered_data(vtx_io_test)
include(CTest)
include(Catch)
set(CMAKE_CATCH_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
catch_discover_tests(vtx_io_test)

