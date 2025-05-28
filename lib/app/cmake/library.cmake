# Lib.
add_library(vtx_app)
configure_target(vtx_app)

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
target_sources(vtx_app
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

# Tests.
file(GLOB_RECURSE SOURCES_TEST "${CMAKE_CURRENT_LIST_DIR}/../test/*")
add_executable(vtx_app_test ${SOURCES_TEST})
configure_target(vtx_app_test)

if (NOT DEFINED _VTX_APP_CONAN)
	target_link_libraries(vtx_app PRIVATE vtx_util)
	target_link_libraries(vtx_app PRIVATE vtx_renderer)
	target_link_libraries(vtx_app PRIVATE vtx_core)
	target_link_libraries(vtx_app PRIVATE vtx_io)
	target_link_libraries(vtx_app PRIVATE vtx_python_binding)
	target_link_libraries(vtx_app_test PRIVATE vtx_util)
	target_link_libraries(vtx_app_test PRIVATE vtx_renderer)
	target_link_libraries(vtx_app_test PRIVATE vtx_core)
	target_link_libraries(vtx_app_test PRIVATE vtx_io)	
	target_link_libraries(vtx_app_test PRIVATE vtx_python_binding)
else()
	target_link_libraries(vtx_app PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_app PRIVATE vtx_renderer::vtx_renderer)
	target_link_libraries(vtx_app PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_app PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_app PRIVATE vtx_python_binding::vtx_python_binding)
	target_link_libraries(vtx_app_test PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_app_test PRIVATE vtx_renderer::vtx_renderer)
	target_link_libraries(vtx_app_test PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_app_test PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_app_test PRIVATE vtx_python_binding::vtx_python_binding)
endif()

target_link_libraries(vtx_app PUBLIC EnTT::EnTT)
target_link_libraries(vtx_app_test PRIVATE vtx_app)
target_link_libraries(vtx_app_test PRIVATE Catch2::Catch2WithMain)

vtx_register_build_directory_copy("${CMAKE_CURRENT_LIST_DIR}/../data" "./data")
vtx_copy_registered_data(vtx_app_test) # allow declared files to be copied on build.

include(CTest)
include(Catch)
catch_discover_tests(vtx_app_test DISCOVERY_MODE PRE_TEST)