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

include(CTest)
include(Catch)

# Util.
add_library(vtx_util)
configure_target(vtx_util)
file(GLOB_RECURSE HEADERS_UTIL ${DIR_LIBS}/util/include/*)
file(GLOB_RECURSE SOURCES_UTIL ${DIR_LIBS}/util/src/*)
target_sources(vtx_util
	PRIVATE ${SOURCES_UTIL}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS ${DIR_LIBS}/util/include FILES ${HEADERS_UTIL})
target_link_libraries(vtx_util PUBLIC glm::glm)
target_link_libraries(vtx_util PUBLIC spdlog::spdlog)
target_link_libraries(vtx_util PRIVATE nlohmann_json::nlohmann_json) # ??
target_link_libraries(vtx_util PUBLIC magic_enum::magic_enum)
target_link_libraries(vtx_util PUBLIC cpr::cpr)

# Util test.
file(GLOB_RECURSE SOURCES_UTIL_TEST ${DIR_LIBS}/util/test/src/*)
add_executable(vtx_util_test ${SOURCES_UTIL_TEST})
configure_target(vtx_util_test)
target_link_libraries(vtx_util_test PRIVATE vtx_util)
target_link_libraries(vtx_util_test PRIVATE nlohmann_json::nlohmann_json) # ??
target_link_libraries(vtx_util_test PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(vtx_util_test)

# Core.
add_library(vtx_core)
configure_target(vtx_core)
file(GLOB_RECURSE HEADERS_CORE ${DIR_LIBS}/core/include/*)
file(GLOB_RECURSE SOURCES_CORE ${DIR_LIBS}/core/src/*)
target_sources(vtx_core
	PRIVATE ${SOURCES_CORE}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS ${DIR_LIBS}/core/include FILES ${HEADERS_CORE})
target_link_libraries(vtx_core PRIVATE vtx_util)

# IO.
add_library(vtx_io)
configure_target(vtx_io)
file(GLOB_RECURSE HEADERS_IO ${DIR_LIBS}/io/include/*)
file(GLOB_RECURSE SOURCES_IO ${DIR_LIBS}/io/src/*)
target_sources(vtx_io
	PRIVATE ${SOURCES_IO}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS ${DIR_LIBS}/io/include FILES ${HEADERS_IO})
target_link_libraries(vtx_io PRIVATE vtx_util)
target_link_libraries(vtx_io PRIVATE vtx_core)
target_link_libraries(vtx_io PRIVATE chemfiles::chemfiles)

# IO test.
add_executable(vtx_io_test ${DIR_LIBS}/io/test/src/main.cpp)
configure_target(vtx_io_test)
target_link_libraries(vtx_io_test PRIVATE vtx_util)
target_link_libraries(vtx_io_test PRIVATE vtx_core)
target_link_libraries(vtx_io_test PRIVATE vtx_io)
target_link_libraries(vtx_io_test PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(vtx_io_test)
add_custom_target(vtx_io_test_copy_data ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${DIR_LIBS}/io/test/data $<TARGET_FILE_DIR:vtx_io_test>/data)
add_dependencies(vtx_io_test vtx_io_test_copy_data)

# Renderer.
add_library(vtx_renderer)
configure_target(vtx_renderer)
file(GLOB_RECURSE HEADERS_RENDERER ${DIR_LIBS}/renderer/include/*)
file(GLOB_RECURSE SOURCES_RENDERER ${DIR_LIBS}/renderer/src/*)
file(GLOB_RECURSE RENDERER_IMGUI_BACKEND ${CMAKE_CURRENT_LIST_DIR}/vendor/*)
file(GLOB_RECURSE SHADERS ${DIR_LIBS}/renderer/shaders/*)
target_sources(vtx_renderer
	PRIVATE ${SOURCES_RENDERER}
	PRIVATE ${SHADERS}
	PRIVATE ${RENDERER_IMGUI_BACKEND}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS ${DIR_LIBS}/renderer/include FILES ${HEADERS_RENDERER})
target_link_libraries(vtx_renderer PRIVATE vtx_util)
target_link_libraries(vtx_renderer PUBLIC glad::glad)

# Renderer test.
add_executable(vtx_renderer_test ${DIR_LIBS}/renderer/test/src/main.cpp)
configure_target(vtx_renderer_test)
target_link_libraries(vtx_renderer_test PRIVATE vtx_util)
target_link_libraries(vtx_renderer_test PRIVATE vtx_renderer)
target_link_libraries(vtx_renderer_test PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(vtx_renderer_test)

# Renderer bench.
file(GLOB_RECURSE SOURCES_BENCH ${DIR_LIBS}/renderer/bench/src/*)
file(GLOB_RECURSE VENDORS_BENCH ${DIR_LIBS}/renderer/bench/vendor/*)
add_executable(vtx_renderer_bench ${SOURCES_BENCH} ${VENDORS_BENCH})
configure_target(vtx_renderer_bench)
target_include_directories(vtx_renderer_bench PRIVATE ${DIR_LIBS}/renderer/bench/vendor)
target_link_libraries(vtx_renderer_bench PRIVATE vtx_util)
target_link_libraries(vtx_renderer_bench PRIVATE vtx_renderer)
target_link_libraries(vtx_renderer_bench PRIVATE vtx_core)
target_link_libraries(vtx_renderer_bench PRIVATE vtx_io)
target_link_libraries(vtx_renderer_bench PRIVATE SDL2::SDL2-static)
target_link_libraries(vtx_renderer_bench PRIVATE SDL2::SDL2main)
target_link_libraries(vtx_renderer_bench PRIVATE imgui::imgui)
target_link_libraries(vtx_renderer_bench PRIVATE stb::stb)
#target_link_libraries(vtx_renderer_bench PRIVATE assimp::assimp)

add_custom_target(vtx_renderer_bench_copy_shader ALL COMMAND ${CMAKE_COMMAND} -E copy_directory	${DIR_LIBS}/renderer/shaders $<TARGET_FILE_DIR:vtx_renderer_bench>/shaders)
add_dependencies(vtx_renderer_bench vtx_renderer_bench_copy_shader)

# App.
add_library(vtx_app)
configure_target(vtx_app)
file(GLOB_RECURSE HEADERS_APP ${DIR_LIBS}/app/include/*)
file(GLOB_RECURSE SOURCES_APP ${DIR_LIBS}/app/src/*)
target_sources(vtx_app
	PRIVATE ${SOURCES_APP}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS ${DIR_LIBS}/app/include FILES ${HEADERS_APP})
target_link_libraries(vtx_app PRIVATE vtx_util)
target_link_libraries(vtx_app PRIVATE vtx_renderer)
target_link_libraries(vtx_app PRIVATE vtx_core)
target_link_libraries(vtx_app PRIVATE vtx_io)
target_link_libraries(vtx_app PUBLIC EnTT::EnTT)

# App test.
file(GLOB_RECURSE SOURCES_APP_TEST ${DIR_LIBS}/app/test/src/*)
add_executable(vtx_app_test ${SOURCES_APP_TEST})
target_link_libraries(vtx_app_test PRIVATE vtx_util)
target_link_libraries(vtx_app_test PRIVATE vtx_renderer)
target_link_libraries(vtx_app_test PRIVATE vtx_core)
target_link_libraries(vtx_app_test PRIVATE vtx_io)
target_link_libraries(vtx_app_test PRIVATE vtx_app)
target_link_libraries(vtx_app_test PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(vtx_app_test)
add_custom_target(vtx_app_test_copy_data ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${DIR_LIBS}/app/test/data $<TARGET_FILE_DIR:vtx_app_test>/data)
add_dependencies(vtx_app_test vtx_app_test_copy_data)

# Python binding.
add_library(vtx_python_binding)
configure_target(vtx_python_binding)
file(GLOB_RECURSE HEADERS_PYTHON_BINDING ${DIR_LIBS}/python_binding/include/*)
file(GLOB_RECURSE SOURCES_PYTHON_BINDING ${DIR_LIBS}/python_binding/src/*)
target_sources(vtx_python_binding
	PRIVATE ${SOURCES_PYTHON_BINDING}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS ${DIR_LIBS}/python_binding/include FILES ${HEADERS_PYTHON_BINDING})
target_link_libraries(vtx_python_binding PRIVATE vtx_util)
target_link_libraries(vtx_python_binding PRIVATE vtx_core)
target_link_libraries(vtx_python_binding PRIVATE vtx_app)
target_link_libraries(vtx_python_binding PRIVATE vtx_io)
target_link_libraries(vtx_python_binding PUBLIC pybind11::pybind11)
target_link_libraries(vtx_python_binding PUBLIC pybind11::embed)
pybind11_add_module(PyTX SHARED ${DIR_LIBS}/python_binding/src/python_binding/binding/vtx_module.cpp)
target_link_libraries(PyTX PRIVATE vtx_util)
target_link_libraries(PyTX PRIVATE vtx_core)
target_link_libraries(PyTX PRIVATE vtx_app)
target_link_libraries(PyTX PRIVATE pybind11::pybind11)
target_link_libraries(PyTX PRIVATE pybind11::embed)
target_link_libraries(PyTX PRIVATE vtx_python_binding)

# Python binding test.
file(GLOB_RECURSE SOURCES_PYTHON_BINDING_TEST ${DIR_LIBS}/python_binding/test/src/*)
add_executable(vtx_python_binding_test ${SOURCES_PYTHON_BINDING_TEST})
configure_target(vtx_python_binding_test)
target_link_libraries(vtx_python_binding_test PRIVATE vtx_util)
target_link_libraries(vtx_python_binding_test PRIVATE vtx_core)
target_link_libraries(vtx_python_binding_test PRIVATE vtx_io)
target_link_libraries(vtx_python_binding_test PRIVATE vtx_app)
target_link_libraries(vtx_python_binding_test PRIVATE vtx_python_binding)
target_link_libraries(vtx_python_binding_test PRIVATE PyTX)
target_link_libraries(vtx_python_binding_test PRIVATE Catch2::Catch2WithMain)
add_custom_target(vtx_python_binding_copy_script ALL COMMAND ${CMAKE_COMMAND} -E copy_directory	${DIR_LIBS}/python_binding/python_script $<TARGET_FILE_DIR:vtx_python_binding_test>/python_script)
add_dependencies(vtx_python_binding_test vtx_python_binding_copy_script)
add_custom_target(vtx_python_binding_test_copy_data ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${DIR_LIBS}/python_binding/test/data $<TARGET_FILE_DIR:vtx_python_binding_test>/data)
add_dependencies(vtx_python_binding_test vtx_python_binding_test_copy_data)
catch_discover_tests(vtx_python_binding_test)

# UI.
configure_qt()
add_library(vtx_ui)
configure_target(vtx_ui)
file(GLOB_RECURSE HEADERS_UI ${DIR_LIBS}/ui/include/*)
file(GLOB_RECURSE SOURCES_UI ${DIR_LIBS}/ui/src/*)
file(GLOB_RECURSE QT_FORMS_UI ${DIR_LIBS}/ui/asset/qt/forms/*.ui)
file(GLOB_RECURSE QT_RESOURCES_UI ${DIR_LIBS}/ui/asset/qt/resources/*.qrc)
target_sources(vtx_ui
	PRIVATE ${SOURCES_UI}
	PRIVATE ${QT_FORMS_UI}
	PRIVATE ${QT_RESOURCES_UI}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS ${DIR_LIBS}/ui/include FILES ${HEADERS_UI})
target_link_libraries(vtx_ui PRIVATE vtx_util)
target_link_libraries(vtx_ui PRIVATE vtx_renderer)
target_link_libraries(vtx_ui PRIVATE vtx_core)
target_link_libraries(vtx_ui PRIVATE vtx_io)
target_link_libraries(vtx_ui PRIVATE vtx_app)
target_link_libraries(vtx_ui PRIVATE vtx_python_binding)
target_link_libraries(vtx_ui PRIVATE PyTX)
target_link_libraries(vtx_ui PUBLIC Qt6::Core)
target_link_libraries(vtx_ui PUBLIC Qt6::Gui)
target_link_libraries(vtx_ui PUBLIC Qt6::Widgets)
target_link_libraries(vtx_ui PUBLIC Qt6::OpenGLWidgets)
if(MSVC)
	target_link_libraries(vtx_ui PUBLIC Qt6::QWindowsIntegrationPlugin)
endif()
apply_qt_options(vtx_ui)

# UI test.
add_executable(vtx_ui_test ${DIR_LIBS}/ui/test/src/main.cpp)
configure_target(vtx_ui_test)
target_link_libraries(vtx_ui_test PRIVATE vtx_util)
target_link_libraries(vtx_ui_test PRIVATE vtx_core)
target_link_libraries(vtx_ui_test PRIVATE vtx_renderer)
target_link_libraries(vtx_ui_test PRIVATE vtx_io)
target_link_libraries(vtx_ui_test PRIVATE vtx_app)
target_link_libraries(vtx_ui_test PRIVATE vtx_python_binding)
target_link_libraries(vtx_ui_test PRIVATE PyTX)
target_link_libraries(vtx_ui_test PRIVATE vtx_ui)
target_link_libraries(vtx_ui_test PRIVATE pybind11::embed)
target_link_libraries(vtx_ui_test PRIVATE Catch2::Catch2WithMain)
add_custom_target(vtx_ui_copy_data ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${DIR_LIBS}/ui/test/data $<TARGET_FILE_DIR:vtx_ui_test>/data)
add_dependencies(vtx_ui_test vtx_ui_copy_data)

catch_discover_tests(vtx_ui_test)

# MdPrep.
include("${DIR_LIBS}/util/cmake/copy_registration.cmake")
include("${DIR_LIBS}/tool/tools/mdprep/cmake/library.cmake")

# MdPrep test.
find_package(vtx-gromacs CONFIG REQUIRED)
include("${DIR_LIBS}/tool/tools/mdprep/test/cmake/library.cmake")
add_dependencies(vtx_tool_mdprep_test vtx_tool_mdprep)

