# Lib.
add_library(vtx_app)
vtx_configure_target(vtx_app)

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
file(GLOB_RECURSE HEADERS_VENDORS "${CMAKE_CURRENT_LIST_DIR}/../vendor/*hpp" "${CMAKE_CURRENT_LIST_DIR}/../vendor/*h")
target_sources(vtx_app
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS}
	PRIVATE FILE_SET vendors_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../vendor" FILES ${HEADERS_VENDORS}
)

# Tests.
file(GLOB_RECURSE SOURCES_TEST "${CMAKE_CURRENT_LIST_DIR}/../test/*")
add_executable(vtx_app_test ${SOURCES_TEST})
vtx_configure_target(vtx_app_test)
vtx_link_cuda(vtx_app_test)

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

target_link_libraries(vtx_app_test PRIVATE vtx_app)
target_link_libraries(vtx_app_test PRIVATE Catch2::Catch2WithMain)

# Vendor libs.
if(WIN32)
	set(VELOPACK_LIB "${CMAKE_CURRENT_LIST_DIR}/../vendor/velopack/lib-static/velopack_libc_win_x64_msvc.lib")
	target_link_libraries(vtx_app PRIVATE ntdll)
else()
	set(VELOPACK_LIB "${CMAKE_CURRENT_LIST_DIR}/../vendor/velopack/lib-static/libvelopack_libc_linux_x64_gnu.a")
endif()
target_link_libraries(vtx_app PRIVATE ${VELOPACK_LIB})

# Declare preprocessor definitions.
if (NOT DEFINED VTX_VERSION_MAJOR)
	set(VTX_VERSION_MAJOR 0)
endif()
if (NOT DEFINED VTX_VERSION_MINOR)
	set(VTX_VERSION_MINOR 0)
endif()
if (NOT DEFINED VTX_VERSION_PATCH)
	set(VTX_VERSION_PATCH 0)
endif()

target_compile_definitions(vtx_app PRIVATE VTX_VERSION_MAJOR=${VTX_VERSION_MAJOR})
target_compile_definitions(vtx_app PRIVATE VTX_VERSION_MINOR=${VTX_VERSION_MINOR})
target_compile_definitions(vtx_app PRIVATE VTX_VERSION_PATCH=${VTX_VERSION_PATCH})

vtx_register_build_directory_copy("${CMAKE_CURRENT_LIST_DIR}/../data" "./data")
vtx_copy_registered_data(vtx_app_test) # allow declared files to be copied on build.

include(CTest)
include(Catch)
catch_discover_tests(vtx_app_test DISCOVERY_MODE PRE_TEST)