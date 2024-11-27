
include ("${CMAKE_CURRENT_LIST_DIR}/vtx_python_binding_copy_files.cmake")

add_library(vtx_python_binding)
configure_target(vtx_python_binding)

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
target_sources(vtx_python_binding
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

if(NOT DEFINED _VTX_PYTHON_BINDING_CONAN)
	target_link_libraries(vtx_python_binding PUBLIC vtx_util)
	target_link_libraries(vtx_python_binding PUBLIC vtx_core)
	target_link_libraries(vtx_python_binding PUBLIC vtx_app)
	target_link_libraries(vtx_python_binding PUBLIC vtx_io)
	target_link_libraries(vtx_python_binding PUBLIC pybind11::pybind11)
	target_link_libraries(vtx_python_binding PUBLIC pybind11::embed)

	pybind11_add_module(vtx_python_bin SHARED "${CMAKE_CURRENT_LIST_DIR}/../src/python_binding/binding/vtx_module.cpp")

	#target_link_libraries(vtx_python_bin PUBLIC vtx_util)
	#target_link_libraries(vtx_python_bin PUBLIC vtx_core)
	#target_link_libraries(vtx_python_bin PUBLIC vtx_app)
	#target_link_libraries(vtx_python_bin PUBLIC pybind11::pybind11)
	#target_link_libraries(vtx_python_bin PUBLIC pybind11::embed)
	target_link_libraries(vtx_python_bin PUBLIC vtx_python_binding)
else()
	target_link_libraries(vtx_python_binding PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_python_binding PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_python_binding PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx_python_binding PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_python_binding PRIVATE pybind11::pybind11)
	target_link_libraries(vtx_python_binding PRIVATE pybind11::embed)

	pybind11_add_module(vtx_python_bin SHARED "${CMAKE_CURRENT_LIST_DIR}/../src/python_binding/binding/vtx_module.cpp")

	target_link_libraries(vtx_python_bin PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_python_bin PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_python_bin PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx_python_bin PRIVATE pybind11::pybind11)
	target_link_libraries(vtx_python_bin PRIVATE pybind11::embed)
	target_link_libraries(vtx_python_bin PRIVATE vtx_python_binding)
endif()
