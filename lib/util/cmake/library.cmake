include("${CMAKE_CURRENT_LIST_DIR}/configure_target.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/copy_registration.cmake")

# Lib.
add_library(vtx_util)
configure_target(vtx_util)

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
target_sources(vtx_util
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

# Tests.
file(GLOB_RECURSE TESTS "${CMAKE_CURRENT_LIST_DIR}/../test/*")
add_executable(vtx_util_test ${TESTS})
configure_target(vtx_util_test)

target_link_libraries(vtx_util PUBLIC glm::glm)
target_link_libraries(vtx_util PUBLIC spdlog::spdlog)
target_link_libraries(vtx_util PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(vtx_util PUBLIC magic_enum::magic_enum)
target_link_libraries(vtx_util PRIVATE cpr::cpr)
target_link_libraries(vtx_util PRIVATE stb::stb)

target_link_libraries(vtx_util_test PRIVATE vtx_util)
target_link_libraries(vtx_util_test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(vtx_util_test PRIVATE nlohmann_json::nlohmann_json)

vtx_copy_registered_data(vtx_util_test) # allow declared files to be copied on build

include(CTest)
include(Catch)
catch_discover_tests(vtx_util_test DISCOVERY_MODE PRE_TEST)