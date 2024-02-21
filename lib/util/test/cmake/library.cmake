set(SOURCES "")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
add_executable(vtx_util_test ${SOURCES})
configure_target(vtx_util_test)

if (NOT DEFINED _VTX_UTIL_TEST_CONAN)
	target_link_libraries(vtx_util_test PRIVATE vtx_util)
else()
	target_link_libraries(vtx_util_test PRIVATE vtx_util::vtx_util)
endif()
target_link_libraries(vtx_util_test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(vtx_util_test PRIVATE nlohmann_json::nlohmann_json)

catch_discover_tests(vtx_util_test)