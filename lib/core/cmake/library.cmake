add_library(vtx_core)
configure_target(vtx_core)

set(HEADERS "")
set(SOURCES "")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
target_sources(vtx_core
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})
	
# Tests.
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../test/src/*")
add_executable(vtx_core_test ${TEST_SOURCES})
configure_target(vtx_core_test)


if (NOT DEFINED _VTX_CORE_CONAN)
	target_link_libraries(vtx_core PRIVATE vtx_util)
	target_link_libraries(vtx_core_test PRIVATE vtx_util)
	
	file(GLOB_RECURSE PDB100_GEN_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../test/pdb100/gen/*")

	# vtx_core_pdb100_gen is a project that aim to generate a datafile containing the pdb100 data on atom coordinates, helixes and strands.
	add_executable(vtx_core_pdb100_gen "${PDB100_GEN_SOURCES}")

	# vtx_core_pdb100_test is a project that'll use data generated from vtx_core_pdb100_gen to compare our secondary structure algorithm to the pdb's helixes and strands. 
	add_executable(vtx_core_pdb100_test "${CMAKE_CURRENT_LIST_DIR}/../test/pdb100/test.cpp;${CMAKE_CURRENT_LIST_DIR}/../test/pdb100/data.cpp")
	
	target_compile_definitions(vtx_core_pdb100_gen PRIVATE PDB100_DATABASE_DIR="${PDB100_DIRECTORY_PATH}")
	target_compile_definitions(vtx_core_pdb100_gen PRIVATE PDB100_OUTPUT_FILE="${CMAKE_CURRENT_LIST_DIR}/../test/pdb100/data.cpp")
	target_link_libraries(vtx_core_pdb100_gen PRIVATE LibArchive::LibArchive)	

	target_link_libraries(vtx_core_pdb100_gen PRIVATE vtx_util)
	target_link_libraries(vtx_core_pdb100_test PRIVATE vtx_core)
	target_link_libraries(vtx_core_pdb100_test PRIVATE Catch2::Catch2WithMain)

else()
	target_link_libraries(vtx_core PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_core_test PRIVATE vtx_util::vtx_util)
endif()

target_link_libraries(vtx_core_test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(vtx_core_test PRIVATE vtx_core)

include(CTest)
include(Catch)
catch_discover_tests(vtx_core_test DISCOVERY_MODE PRE_TEST)
