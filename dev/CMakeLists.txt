set(DIR_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

function(configure_target p_target)
	if(CMAKE_COMPILER_IS_GNUCC)
		target_compile_options(${p_target} PRIVATE "-Wpedantic")
		target_compile_options(${p_target} PRIVATE "-Wall")
	elseif(MSVC)
		# General.
		target_compile_options(${p_target} PRIVATE "/W3")  # Warning level 3.
		target_compile_options(${p_target} PRIVATE "/WX")  # Warnings as errors.
		target_compile_options(${p_target} PRIVATE "/MP")  # Multicore compilation.
		target_compile_options(${p_target} PRIVATE "/sdl") # Additional Security Checks.	
		target_compile_options(${p_target} PRIVATE "/utf-8")
		target_compile_options(${p_target} PRIVATE "/fp:fast") # Floating Point Model.
		# Optimization.
		target_compile_options(${p_target} PRIVATE "$<$<CONFIG:Release>:/O2>")
		target_compile_options(${p_target} PRIVATE "$<$<CONFIG:Release>:/Ob2>")
		target_compile_options(${p_target} PRIVATE "$<$<CONFIG:Release>:/Ot>")
		target_compile_options(${p_target} PRIVATE "$<$<CONFIG:Release>:/Oi>")
	endif()
	# Force _DEBUG preprocessor on all plateforms.
	if(DEFINED CMAKE_BUILD_TYPE)
		if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
			target_compile_definitions(${p_target} PRIVATE _DEBUG)
		endif()
	endif()	
endfunction()
function (configure_qt)
	set(CMAKE_AUTOMOC ON PARENT_SCOPE)
	set(CMAKE_AUTORCC ON PARENT_SCOPE)
	set(CMAKE_AUTOUIC ON PARENT_SCOPE)
	set(CMAKE_AUTOUIC_SEARCH_PATHS ${DIR_LIBS}/ui/asset/qt/forms PARENT_SCOPE)
	#set(CMAKE_AUTOGEN_TARGETS_FOLDER "autogen")
	set_property(GLOBAL PROPERTY USE_FOLDERS ON PARENT_SCOPE)
	set(CMAKE_INCLUDE_CURRENT_DIR ON PARENT_SCOPE)
	#set(CMAKE_POSITION_INDEPENDENT_CODE ON)	
endfunction()
function (apply_qt_options target)
	set_property(TARGET ${target} PROPERTY AUTOUIC_OPTIONS "--no-autoconnection")
	add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)	
endfunction()


message("vtx")
cmake_minimum_required(VERSION 3.23)

project(vtx)

find_package(glm  REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(chemfiles CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(stb CONFIG CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(pybind11 CONFIG REQUIRED)
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

include(CTest)
include(Catch)

# Util.
include("${DIR_LIBS}/util/cmake/library.cmake")

# Util test.
include("${DIR_LIBS}/util/test/cmake/library.cmake")

# Core.
include("${DIR_LIBS}/core/cmake/library.cmake")

# IO.
include("${DIR_LIBS}/io/cmake/library.cmake")

# IO test.
include("${DIR_LIBS}/io/test/cmake/library.cmake")

# Renderer.
include("${DIR_LIBS}/renderer/cmake/library.cmake")

# Renderer test.
include("${DIR_LIBS}/renderer/test/cmake/library.cmake")


# _________________________________________________________________
# Renderer bench.




#include("${DIR_LIBS}/renderer/bench/cmake/library.cmake")

## Should work but "lib/renderer/bench/vendor/imnodes/imnodes.*" files are not copied by conan and are actually in the git tracking tree. We might want to rework the vendor part of this project. I leave if commented for the time being.




#file(GLOB_RECURSE SOURCES_BENCH ${DIR_LIBS}/renderer/bench/src/*)
#file(GLOB_RECURSE VENDORS_BENCH ${DIR_LIBS}/renderer/bench/vendor/*)
#add_executable(vtx_renderer_bench ${SOURCES_BENCH} ${VENDORS_BENCH})
#configure_target(vtx_renderer_bench)
#target_include_directories(vtx_renderer_bench PRIVATE ${DIR_LIBS}/renderer/bench/vendor)
#target_link_libraries(vtx_renderer_bench PRIVATE vtx_util)
#target_link_libraries(vtx_renderer_bench PRIVATE vtx_renderer)
#target_link_libraries(vtx_renderer_bench PRIVATE vtx_core)
#target_link_libraries(vtx_renderer_bench PRIVATE vtx_io)
#target_link_libraries(vtx_renderer_bench PRIVATE SDL2::SDL2-static)
#target_link_libraries(vtx_renderer_bench PRIVATE SDL2::SDL2main)
#target_link_libraries(vtx_renderer_bench PRIVATE imgui::imgui)
#target_link_libraries(vtx_renderer_bench PRIVATE stb::stb)
##target_link_libraries(vtx_renderer_bench PRIVATE assimp::assimp)
#
#add_custom_target(vtx_renderer_bench_copy_shader ALL COMMAND ${CMAKE_COMMAND} -E copy_directory	${DIR_LIBS}/renderer/shaders #$<TARGET_FILE_DIR:vtx_renderer_bench>/shaders)
#add_dependencies(vtx_renderer_bench vtx_renderer_bench_copy_shader)



# _________________________________________________________________

# App.
include("${DIR_LIBS}/app/cmake/library.cmake")

# App test.
include("${DIR_LIBS}/app/test/cmake/library.cmake")

# Python binding.
include("${DIR_LIBS}/python_binding/cmake/library.cmake")

# Python binding test.
include("${DIR_LIBS}/python_binding/test/cmake/library.cmake")

# UI.
include("${DIR_LIBS}/ui/cmake/library.cmake")

# UI test.
include("${DIR_LIBS}/ui/test/cmake/library.cmake")

# MdPrep.
include("${DIR_LIBS}/tool/tools/mdprep/cmake/library.cmake")

# MdPrep test.
include("${DIR_LIBS}/tool/tools/mdprep/test/cmake/library.cmake")

