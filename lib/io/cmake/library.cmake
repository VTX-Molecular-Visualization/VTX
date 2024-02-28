add_library(vtx_io)
configure_target(vtx_io)

set(HEADERS "")
set(SOURCES "")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
target_sources(vtx_io
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

if (NOT DEFINED _VTX_IO_CONAN)
	target_link_libraries(vtx_io vtx_util)
	target_link_libraries(vtx_io vtx_core)
	target_link_libraries(vtx_io chemfiles::chemfiles)
else()
	target_link_libraries(vtx_io PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_io PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_io PRIVATE chemfiles::chemfiles)
endif()
