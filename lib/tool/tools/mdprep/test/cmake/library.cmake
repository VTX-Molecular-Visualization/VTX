

set(HEADERS "")
set(SOURCES "")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/../include/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")


add_executable(vtx_tool_mdprep_test ${SOURCES})
configure_target(vtx_tool_mdprep_test)


if (NOT DEFINED _VTX_MDPREP_CONAN)
	target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_util)
	target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_app)
	target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_core)
	target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_tool_mdprep)
else()
	target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_app::vtx_app)
	target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_tool_mdprep_test PRIVATE vtx_tool_mdprep::vtx_tool_mdprep)
endif()
target_link_libraries(vtx_tool_mdprep_test PRIVATE Qt6::Core)			 # I feel weird about linked lib not being 
target_link_libraries(vtx_tool_mdprep_test PRIVATE Qt6::Gui)			 # carried through packages, forcing me
target_link_libraries(vtx_tool_mdprep_test PRIVATE Qt6::Widgets)		 # to link Qt6 explicitly. I don't like it
target_link_libraries(vtx_tool_mdprep_test PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(vtx_tool_mdprep_test PRIVATE Catch2::Catch2WithMain)

target_include_directories(vtx_tool_mdprep_test PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include)

include(CTest)
include(Catch)
catch_discover_tests(vtx_tool_mdprep_test)
vtx_register_build_directory_copy("${CMAKE_CURRENT_LIST_DIR}/../data" "data")

vtx_copy_registered_data(vtx_tool_mdprep_test $<TARGET_FILE_DIR:vtx_tool_mdprep_test>)