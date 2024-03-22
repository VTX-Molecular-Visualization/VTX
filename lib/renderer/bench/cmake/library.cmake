include ("${CMAKE_CURRENT_LIST_DIR}/copy_data.cmake")
set(_VTX_RENDERER_BENCH_VENDOR_DIR "${CMAKE_CURRENT_LIST_DIR}/../vendor")
set(SOURCES "")
# There is currently 2 vendor type of files : those that are statically bound to the project tree (imnodes) referred here as VENDOR_STATIC
# And those copied by the build system, referred here as VENDORS_DYNAMIC. 
set(VENDORS_STATIC "")
set(VENDORS_DYNAMIC "")

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
file(GLOB_RECURSE VENDORS_STATIC "${_VTX_RENDERER_BENCH_VENDOR_DIR}/imnodes/*")
file(GLOB_RECURSE VENDORS_DYNAMIC ./vendor/*)
add_executable(vtx_renderer_bench ${SOURCES} ${VENDORS_STATIC} ${VENDORS_DYNAMIC})
configure_target(vtx_renderer_bench)

if(NOT DEFINED _VTX_RENDERER_BENCH_CONAN)
	target_include_directories(vtx_renderer_bench PRIVATE "${_VTX_RENDERER_BENCH_VENDOR_DIR}" ./vendor)

	target_link_libraries(vtx_renderer_bench PRIVATE vtx_util)
	target_link_libraries(vtx_renderer_bench PRIVATE vtx_renderer)
	target_link_libraries(vtx_renderer_bench PRIVATE vtx_core)
	target_link_libraries(vtx_renderer_bench PRIVATE vtx_io)
	target_link_libraries(vtx_renderer_bench PRIVATE SDL2::SDL2-static)
	target_link_libraries(vtx_renderer_bench PRIVATE SDL2::SDL2main)
	target_link_libraries(vtx_renderer_bench PRIVATE imgui::imgui)
	target_link_libraries(vtx_renderer_bench PRIVATE stb::stb)
	#target_link_libraries(vtx_renderer_bench PRIVATE assimp::assimp)
else()
	target_include_directories(vtx_renderer_bench PRIVATE vendor)

	target_link_libraries(vtx_renderer_bench PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_renderer_bench PRIVATE vtx_renderer::vtx_renderer)
	target_link_libraries(vtx_renderer_bench PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_renderer_bench PRIVATE vtx_io::vtx_io)
	target_link_libraries(vtx_renderer_bench PRIVATE SDL2::SDL2-static)
	target_link_libraries(vtx_renderer_bench PRIVATE SDL2::SDL2main)
	target_link_libraries(vtx_renderer_bench PRIVATE imgui::imgui)
	target_link_libraries(vtx_renderer_bench PRIVATE stb::stb)
	#target_link_libraries(vtx_renderer_bench PRIVATE assimp::assimp)
endif()

vtx_copy_registered_data(vtx_renderer_bench)

