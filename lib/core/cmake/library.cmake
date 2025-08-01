include("${CMAKE_CURRENT_LIST_DIR}/vtx_core_copy_files.cmake")

add_library(vtx_core)
vtx_configure_target(vtx_core)

set(HEADERS "")
set(SOURCES "")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
target_sources(vtx_core
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

if (NOT DEFINED _VTX_CORE_CONAN)
	target_link_libraries(vtx_core PRIVATE vtx_util)
else()
	target_link_libraries(vtx_core PRIVATE vtx_util::vtx_util)
endif()

vtx_copy_registered_data(vtx_core) # allow declared files to be copied on build