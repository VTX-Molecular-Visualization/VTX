include("${CMAKE_CURRENT_LIST_DIR}/configure_target.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/copy_registration.cmake")

add_library(vtx_util)
configure_target(vtx_util)

set(HEADERS "")
set(SOURCES "")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
target_sources(vtx_util
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

if (NOT DEFINED _VTX_UTIL_CONAN)
	target_link_libraries(vtx_util glm::glm)
	target_link_libraries(vtx_util spdlog::spdlog)
	target_link_libraries(vtx_util nlohmann_json::nlohmann_json)
	target_link_libraries(vtx_util magic_enum::magic_enum)
	target_link_libraries(vtx_util cpr::cpr)
else()
	target_link_libraries(vtx_util PRIVATE glm::glm)
	target_link_libraries(vtx_util PRIVATE spdlog::spdlog)
	target_link_libraries(vtx_util PRIVATE nlohmann_json::nlohmann_json)
	target_link_libraries(vtx_util PRIVATE magic_enum::magic_enum)
	target_link_libraries(vtx_util PRIVATE cpr::cpr)
endif()