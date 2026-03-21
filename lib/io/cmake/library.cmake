# Lib.
add_library(vtx_io)
vtx_configure_target(vtx_io)

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
target_sources(vtx_io
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

# Tests.
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../test/src/*")
add_executable(vtx_io_test ${TEST_SOURCES})
set_property(TARGET vtx_io_test PROPERTY FOLDER "test")
vtx_configure_target(vtx_io_test)

if (NOT DEFINED _VTX_IO_CONAN)
	target_link_libraries(vtx_io PUBLIC vtx_util)
	target_link_libraries(vtx_io PUBLIC vtx_core)		
	target_link_libraries(vtx_io_test PRIVATE vtx_util)
	target_link_libraries(vtx_io_test PRIVATE vtx_core)	
else()
	target_link_libraries(vtx_io PUBLIC vtx_util::vtx_util)
	target_link_libraries(vtx_io PUBLIC vtx_core::vtx_core)
	target_link_libraries(vtx_io_test PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_io_test PRIVATE vtx_core::vtx_core)
endif()

target_link_libraries(vtx_io PUBLIC chemfiles::chemfiles)
target_link_libraries(vtx_io_test PRIVATE vtx_io)
target_link_libraries(vtx_io_test PRIVATE Catch2::Catch2WithMain)

vtx_copy_directory(vtx_io_test "${CMAKE_CURRENT_LIST_DIR}/../data" "$<TARGET_FILE_DIR:vtx_io_test>/data")

include(CTest)
include(Catch)
catch_discover_tests(vtx_io_test DISCOVERY_MODE PRE_TEST)
