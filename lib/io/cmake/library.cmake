# Lib.
add_library(vtx_io)
configure_target(vtx_io)

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
target_sources(vtx_io
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

# Tests.
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../test/src/*")
add_executable(vtx_io_test ${TEST_SOURCES})
configure_target(vtx_io_test)

if (NOT DEFINED _VTX_IO_CONAN)
	target_link_libraries(vtx_io PRIVATE vtx_util)
	target_link_libraries(vtx_io PRIVATE vtx_core)		
	target_link_libraries(vtx_io_test PRIVATE vtx_util)
	target_link_libraries(vtx_io_test PRIVATE vtx_core)		
	target_compile_definitions(vtx_io_test PRIVATE PDB_DATABASE_DIR="C:/Users/Valen/data/pdb100_subset")
else()
	target_link_libraries(vtx_io PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_io PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_io_test PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_io_test PRIVATE vtx_core::vtx_core)
endif()



# WIP
target_link_libraries(vtx_io_test PRIVATE Boost::headers)
target_include_directories(vtx_io_test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../test/include;")


file(GLOB_RECURSE NEW_PROCESS_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../test/new_process/*")
add_executable(vtx_io_new_process ${NEW_PROCESS_SOURCES})
target_include_directories(vtx_io_new_process PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../test/include;")
configure_target(vtx_io_new_process)
target_link_libraries(vtx_io_new_process PRIVATE vtx_util)
target_link_libraries(vtx_io_new_process PRIVATE vtx_core)	
target_link_libraries(vtx_io_new_process PRIVATE LibArchive::LibArchive)	
target_link_libraries(vtx_io_new_process PRIVATE vtx_io)
target_link_libraries(vtx_io_new_process PRIVATE Boost::headers)



# !WIP

target_link_libraries(vtx_io PRIVATE chemfiles::chemfiles)
target_link_libraries(vtx_io_test PRIVATE vtx_io)
target_link_libraries(vtx_io_test PRIVATE Catch2::Catch2WithMain)

vtx_register_build_directory_copy("${CMAKE_CURRENT_LIST_DIR}/../data" "./data")
vtx_copy_registered_data(vtx_io_test)

include(CTest)
include(Catch)
catch_discover_tests(vtx_io_test DISCOVERY_MODE PRE_TEST)