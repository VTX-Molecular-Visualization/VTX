
include ("${CMAKE_CURRENT_LIST_DIR}/vtx_python_binding_copy_files.cmake")

add_library(vtx_python_binding)
configure_target(vtx_python_binding)
add_library(vtx_python_binding_no_opengl)
configure_target(vtx_python_binding_no_opengl)

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
file(GLOB_RECURSE SOURCES_TEST "${CMAKE_CURRENT_LIST_DIR}/../test/src/*")
target_sources(vtx_python_binding
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})
target_sources(vtx_python_binding_no_opengl
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

add_executable(vtx_python_binding_test "${SOURCES};${SOURCES_TEST}")
target_include_directories(vtx_python_binding_test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../include")

configure_target(vtx_python_binding_test)

pybind11_add_module(vtx_python_bin SHARED "${CMAKE_CURRENT_LIST_DIR}/../src/python_binding/binding/vtx_module.cpp")
pybind11_add_module(vtx_python_bin_no_opengl SHARED "${CMAKE_CURRENT_LIST_DIR}/../src/python_binding/binding/vtx_module.cpp")

if(NOT DEFINED _VTX_PYTHON_BINDING_CONAN)
	target_link_libraries(vtx_python_binding PUBLIC vtx_util)
	target_link_libraries(vtx_python_binding PUBLIC vtx_core)
	target_link_libraries(vtx_python_binding PUBLIC vtx_app)
	target_link_libraries(vtx_python_binding PUBLIC vtx_io)
	
	target_link_libraries(vtx_python_binding_no_opengl PUBLIC vtx_util)
	target_link_libraries(vtx_python_binding_no_opengl PUBLIC vtx_core)
	target_link_libraries(vtx_python_binding_no_opengl PUBLIC poneyponey)
	target_link_libraries(vtx_python_binding_no_opengl PUBLIC vtx_io)

	target_link_libraries(vtx_python_bin PUBLIC vtx_util)
	target_link_libraries(vtx_python_bin PUBLIC vtx_core)
	target_link_libraries(vtx_python_bin PUBLIC vtx_app)
	
	target_link_libraries(vtx_python_bin_no_opengl PRIVATE vtx_util)
	target_link_libraries(vtx_python_bin_no_opengl PRIVATE vtx_core)
	target_link_libraries(vtx_python_bin_no_opengl PRIVATE vtx_renderer_no_opengl)
	target_link_libraries(vtx_python_bin_no_opengl PRIVATE poneyponey)
	target_link_libraries(vtx_python_bin_no_opengl PRIVATE vtx_io)

	target_link_libraries(vtx_python_binding_test PRIVATE vtx_util)
	target_link_libraries(vtx_python_binding_test PRIVATE poneyponey)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_core)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_io)
else()
	target_link_libraries(vtx_python_binding PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_python_binding PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_python_binding PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx_python_binding PRIVATE vtx_io::vtx_io)
	
	target_link_libraries(vtx_python_binding_no_opengl PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_python_binding_no_opengl PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_python_binding_no_opengl PRIVATE vtx_app::poneyponey)
	target_link_libraries(vtx_python_binding_no_opengl PRIVATE vtx_io::vtx_io)
	
	target_link_libraries(vtx_python_bin PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_python_bin PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_python_bin PRIVATE vtx_renderer::vtx_renderer)
	target_link_libraries(vtx_python_bin PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx_python_bin PRIVATE vtx_io::vtx_io)
	
	target_link_libraries(vtx_python_bin_no_opengl PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_python_bin_no_opengl PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_python_bin_no_opengl PRIVATE vtx_renderer::vtx_renderer_no_opengl)
	target_link_libraries(vtx_python_bin_no_opengl PRIVATE vtx_app::poneyponey)
	target_link_libraries(vtx_python_bin_no_opengl PRIVATE vtx_io::vtx_io)

	target_link_libraries(vtx_python_binding_test PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_renderer::vtx_renderer_no_opengl)
	target_link_libraries(vtx_python_binding_test PRIVATE vtx_app::poneyponey)
endif()

# TODO : are those lines usefull ?
target_link_libraries(vtx_python_binding PUBLIC EnTT::EnTT)
target_link_libraries(vtx_python_binding_no_opengl PUBLIC EnTT::EnTT)
target_link_libraries(vtx_python_binding_test PUBLIC EnTT::EnTT)
target_link_libraries(vtx_python_bin PUBLIC EnTT::EnTT)
# !TODO


target_link_libraries(vtx_python_binding PRIVATE pybind11::pybind11)
target_link_libraries(vtx_python_binding PRIVATE pybind11::embed)

target_link_libraries(vtx_python_binding_no_opengl PRIVATE pybind11::pybind11)
target_link_libraries(vtx_python_binding_no_opengl PRIVATE pybind11::embed)

target_link_libraries(vtx_python_binding_test PRIVATE pybind11::pybind11)
target_link_libraries(vtx_python_binding_test PRIVATE pybind11::embed)

target_link_libraries(vtx_python_bin PUBLIC vtx_python_binding)
target_link_libraries(vtx_python_bin PRIVATE pybind11::pybind11)
target_link_libraries(vtx_python_bin PRIVATE pybind11::embed)

target_link_libraries(vtx_python_bin_no_opengl PUBLIC vtx_python_binding_no_opengl)
target_link_libraries(vtx_python_bin_no_opengl PRIVATE pybind11::pybind11)
target_link_libraries(vtx_python_bin_no_opengl PRIVATE pybind11::embed)
target_link_libraries(vtx_python_binding_test PRIVATE vtx_python_bin_no_opengl) # TODO : Useful ?
target_link_libraries(vtx_python_binding_test PRIVATE Catch2::Catch2WithMain)

# All other find_package call
vtx_register_build_directory_copy("${CMAKE_CURRENT_LIST_DIR}/../test/data" "./data")
vtx_copy_registered_data(vtx_python_binding_test)

include(CTest)
include(Catch)

catch_discover_tests(vtx_python_binding_test DISCOVERY_MODE PRE_TEST)