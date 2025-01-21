
include ("${CMAKE_CURRENT_LIST_DIR}/vtx_tool_mdprep_copy_files.cmake")

add_library(vtx_tool_mdprep)
configure_target(vtx_tool_mdprep)

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
message("mdprep headers : <${HEADERS}>")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")

target_sources(vtx_tool_mdprep
	PRIVATE ${SOURCES}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" FILES ${HEADERS})

add_resources(vtx_tool_mdprep ${CMAKE_CURRENT_LIST_DIR}/../asset z_vtx_tool_mdprep)

if (NOT DEFINED _VTX_MDPREP_CONAN)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_util)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_core)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_app)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_ui_qt)
else()
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx_tool_mdprep PRIVATE vtx_ui_qt::vtx_ui_qt)
endif()
target_link_libraries(vtx_tool_mdprep PRIVATE Qt6::Core)
target_link_libraries(vtx_tool_mdprep PRIVATE Qt6::Gui)
target_link_libraries(vtx_tool_mdprep PRIVATE Qt6::Widgets)
target_link_libraries(vtx_tool_mdprep PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(vtx_tool_mdprep PRIVATE re2::re2)

target_link_libraries(vtx_tool_mdprep PUBLIC EnTT::EnTT)  # TODO : see <lib/python_binding/cmake/library.cmake> for details

#vtx_copy_registered_data(vtx_tool_mdprep)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../test/src/*")


add_executable(vtx_tool_mdprep_test ${SOURCES} )
configure_target(vtx_tool_mdprep_test)

target_include_directories(vtx_tool_mdprep_test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../include")

if (NOT DEFINED _VTX_MDPREP_CONAN)
	target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_util)
	target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_io)
	target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_core)
	target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_app_no_opengl)
	target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_renderer_no_opengl)
else()
	target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_app::vtx_app_no_opengl)
	# It seems that, for now, adding the link toward these following packages confuses linux linker. Weird.
	# target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_renderer::vtx_renderer_no_opengl)
endif()
target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_tool_mdprep)
target_link_libraries(vtx_tool_mdprep_test PRIVATE Qt6::Core)			 # I feel weird about linked lib not being 
target_link_libraries(vtx_tool_mdprep_test PRIVATE Qt6::Gui)			 # carried through packages, forcing me
target_link_libraries(vtx_tool_mdprep_test PRIVATE Qt6::Widgets)		 # to link Qt6 explicitly. I don't like it
target_link_libraries(vtx_tool_mdprep_test PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(vtx_tool_mdprep_test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(vtx_tool_mdprep_test PRIVATE re2::re2)

target_link_libraries(vtx_tool_mdprep_test PUBLIC EnTT::EnTT)  # TODO : see <lib/python_binding/cmake/library.cmake> for details

include(CTest)
include(Catch)
catch_discover_tests(vtx_tool_mdprep_test DISCOVERY_MODE PRE_TEST)
vtx_register_build_directory_copy("${CMAKE_CURRENT_LIST_DIR}/../test/data" "data")

vtx_copy_registered_data(vtx_tool_mdprep_test $<TARGET_FILE_DIR:vtx_tool_mdprep_test>) # allow declared files to be copied on build