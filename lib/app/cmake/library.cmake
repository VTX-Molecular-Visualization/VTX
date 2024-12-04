# Lib.
add_library(vtx_app)
configure_target(vtx_app)

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
target_sources(vtx_app
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})
	
add_library(vtx_app_no_opengl)
configure_target(vtx_app_no_opengl)

target_sources(vtx_app_no_opengl
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

# Tests.
# I think OpenGL from {vtx_app->vtx_renderer} prevents us to link the target vtx_renderer_no_opengl normally 
# So instead of app_test linking against app, we compile source from app directly into app_test. 
# This require for libraires that are private to app to be linked with app_test as well.
# Moreover, we need to keep in mind that we are thus testing the code, not the project.

file(GLOB_RECURSE SOURCES_TEST "${CMAKE_CURRENT_LIST_DIR}/../test/*")
add_executable(vtx_app_test "${SOURCES_TEST}")
#add_executable(vtx_app_test "${SOURCES_TEST};${SOURCES}")
#target_include_directories(vtx_app_test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../include")

configure_target(vtx_app_test)

if (NOT DEFINED _VTX_APP_CONAN)
	target_link_libraries(vtx_app PRIVATE vtx_util)
	target_link_libraries(vtx_app PRIVATE vtx_renderer)
	target_link_libraries(vtx_app PRIVATE vtx_core)
	target_link_libraries(vtx_app PRIVATE vtx_io)
	target_link_libraries(vtx_app_no_opengl PRIVATE vtx_util)
	target_link_libraries(vtx_app_no_opengl PRIVATE vtx_renderer_no_opengl)
	target_link_libraries(vtx_app_no_opengl PRIVATE vtx_core)
	target_link_libraries(vtx_app_no_opengl PRIVATE vtx_io)
	target_link_libraries(vtx_app_test PRIVATE vtx_util)
	target_link_libraries(vtx_app_test PRIVATE vtx_renderer_no_opengl)
	target_link_libraries(vtx_app_test PRIVATE vtx_core)
	target_link_libraries(vtx_app_test PRIVATE vtx_io)	
else()
	target_link_libraries(vtx_app PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_app PUBLIC vtx_renderer::vtx_renderer)
	target_link_libraries(vtx_app PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_app PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_app_no_opengl PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_app_no_opengl PUBLIC vtx_renderer::vtx_renderer_no_opengl)
	target_link_libraries(vtx_app_no_opengl PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_app_no_opengl PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_app_test PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_app_test PUBLIC vtx_renderer::vtx_renderer_no_opengl)
	target_link_libraries(vtx_app_test PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_app_test PRIVATE vtx_io::vtx_io)
endif()

target_link_libraries(vtx_app PUBLIC EnTT::EnTT)
target_link_libraries(vtx_app_no_opengl PUBLIC EnTT::EnTT)
target_link_libraries(vtx_app_test PRIVATE vtx_app_no_opengl)
target_link_libraries(vtx_app_test PRIVATE Catch2::Catch2WithMain)

vtx_register_build_directory_copy("${CMAKE_CURRENT_LIST_DIR}/../data" "./data")
vtx_copy_registered_data(vtx_app_test) # allow declared files to be copied on build

include(CTest)
include(Catch)


catch_discover_tests(vtx_app_test DISCOVERY_MODE PRE_TEST)