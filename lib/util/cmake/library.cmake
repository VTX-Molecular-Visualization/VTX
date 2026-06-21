include("${CMAKE_CURRENT_LIST_DIR}/vtx_configure_target.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/vtx_copy.cmake")

# Lib.
add_library(vtx_util)
add_library(vtx_util::vtx_util ALIAS vtx_util)
vtx_configure_target(vtx_util)

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
target_sources(vtx_util
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

# Tests.
file(GLOB_RECURSE TESTS "${CMAKE_CURRENT_LIST_DIR}/../test/*")
add_executable(vtx_util_test ${TESTS})
set_property(TARGET vtx_util_test PROPERTY FOLDER "test")
vtx_configure_target(vtx_util_test)

target_link_libraries(vtx_util PUBLIC glm::glm)
target_link_libraries(vtx_util PUBLIC spdlog::spdlog)
target_link_libraries(vtx_util PUBLIC magic_enum::magic_enum)
target_link_libraries(vtx_util PUBLIC EnTT::EnTT)
target_link_libraries(vtx_util PUBLIC assimp::assimp)
target_link_libraries(vtx_util PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(vtx_util PRIVATE cpr::cpr)
target_link_libraries(vtx_util PRIVATE stb::stb)
target_link_libraries(vtx_util PRIVATE tinyexr::tinyexr)
target_link_libraries(vtx_util PRIVATE sago::platform_folders)

target_link_libraries(vtx_util_test PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_util_test PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(vtx_util_test PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(vtx_util_test DISCOVERY_MODE PRE_TEST)
