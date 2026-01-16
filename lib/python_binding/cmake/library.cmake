

add_library(vtx_python_binding)
vtx_configure_target(vtx_python_binding)

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
file(GLOB_RECURSE SOURCES_TEST "${CMAKE_CURRENT_LIST_DIR}/../test/src/*")
target_sources(vtx_python_binding
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

add_executable(vtx_python_binding_test "${SOURCES};${SOURCES_TEST}")

if (UNIX)
	# This allow python submodules to find python symbols on unix, as explained here : https://stackoverflow.com/questions/67891197/ctypes-cpython-39-x86-64-linux-gnu-so-undefined-symbol-pyfloat-type-in-embedd
	target_link_options(vtx_python_binding_test PUBLIC -Xlinker -export-dynamic)
endif()

target_include_directories(vtx_python_binding_test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../include")

vtx_configure_target(vtx_python_binding_test)


if(NOT DEFINED _VTX_PYTHON_BINDING_CONAN)
	target_link_libraries(vtx_python_binding PUBLIC vtx_util)
	target_link_libraries(vtx_python_binding PUBLIC vtx_core)
	target_link_libraries(vtx_python_binding PUBLIC vtx_io)

	target_link_libraries(vtx_python_binding_test PRIVATE vtx_util)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_core)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_io)
else()
	target_link_libraries(vtx_python_binding PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_python_binding PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_python_binding PRIVATE vtx_io::vtx_io)

	target_link_libraries(vtx_python_binding_test PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_io::vtx_io)
endif()

target_link_libraries(vtx_python_binding PUBLIC pybind11::module)
target_link_libraries(vtx_python_binding_test PRIVATE Python3::Python)
target_link_libraries(vtx_python_binding PRIVATE Python3::Python Python3::Module)

target_link_libraries(vtx_python_binding_test PRIVATE vtx_python_binding)
target_link_libraries(vtx_python_binding_test PRIVATE Catch2::Catch2WithMain)

target_compile_definitions(vtx_python_binding PRIVATE CPYTHON_VERSION_MAJOR="${CPYTHON_VERSION_MAJOR}")
target_compile_definitions(vtx_python_binding PRIVATE CPYTHON_VERSION_MINOR="${CPYTHON_VERSION_MINOR}")
target_compile_definitions(vtx_python_binding PRIVATE CPYTHON_VERSION_PATCH="${CPYTHON_VERSION_PATCH}")

target_compile_definitions(vtx_python_binding_test PRIVATE CPYTHON_VERSION_MAJOR="${CPYTHON_VERSION_MAJOR}")
target_compile_definitions(vtx_python_binding_test PRIVATE CPYTHON_VERSION_MINOR="${CPYTHON_VERSION_MINOR}")
target_compile_definitions(vtx_python_binding_test PRIVATE CPYTHON_VERSION_PATCH="${CPYTHON_VERSION_PATCH}")

get_target_property(VTX_PY_LINK_LIBS vtx_python_binding LINK_LIBRARIES)

message("VTX - vtx_python_binding links: ${VTX_PY_LINK_LIBS}")
get_target_property(VTX_PY_TEST_LINK_LIBS vtx_python_binding_test LINK_LIBRARIES)
message("VTX - vtx_python_binding_test links: ${VTX_PY_TEST_LINK_LIBS}")
include ("${CMAKE_CURRENT_LIST_DIR}/vtx_python_binding_copy_files.cmake")# All other find_package call
vtx_copy_registered_data(vtx_python_binding)
vtx_clear_registered_copies()

vtx_register_build_directory_copy("${CMAKE_CURRENT_LIST_DIR}/../test/data" "./data")
vtx_copy_registered_data(vtx_python_binding_test)

include(CTest)
include(Catch)

catch_discover_tests(vtx_python_binding_test DISCOVERY_MODE PRE_TEST)