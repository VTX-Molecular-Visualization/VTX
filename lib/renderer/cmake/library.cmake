include("${CMAKE_CURRENT_LIST_DIR}/vtx_link_cuda.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/vtx_renderer_copy_shaders.cmake")

# Cuda.
include(CheckLanguage)
check_language(CUDA)
set(VTX_CUDA_ENABLED OFF)
if(CMAKE_CUDA_COMPILER)
	enable_language(CUDA)
	find_package(CUDAToolkit)
	if(CUDAToolkit_FOUND)
		set(VTX_CUDA_ENABLED ON)
	else()
		message(STATUS "CUDA toolkit not found")
	endif()
else()
	message(STATUS "CUDA not found")
endif()

# Lib.
add_library(vtx_renderer)
add_library(vtx_renderer::vtx_renderer ALIAS vtx_renderer)
vtx_configure_target(vtx_renderer)

file(GLOB_RECURSE HEADERS_PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../include/public/*")
file(GLOB_RECURSE HEADERS_PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../include/private/renderer/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/renderer/*")
file(GLOB_RECURSE HEADERS_VENDORS "${CMAKE_CURRENT_LIST_DIR}/../vendor/glad/*.h")
file(GLOB_RECURSE SOURCES_VENDORS "${CMAKE_CURRENT_LIST_DIR}/../vendor/glad/*.c")
file(GLOB_RECURSE SHADERS "${CMAKE_CURRENT_LIST_DIR}/../shaders/*")
if(VTX_CUDA_ENABLED)
	file(GLOB_RECURSE HEADERS_PRIVATE_BCS "${CMAKE_CURRENT_LIST_DIR}/../include/private/bcs/*")
	file(GLOB_RECURSE SOURCES_BCS "${CMAKE_CURRENT_LIST_DIR}/../src/bcs/*")
	file(GLOB_RECURSE HEADERS_VENDORS_CUDA_HELPER "${CMAKE_CURRENT_LIST_DIR}/../vendor/cuda_helper/*.h")
	list(APPEND HEADERS_PRIVATE ${HEADERS_PRIVATE_BCS})
	list(APPEND SOURCES ${SOURCES_BCS})
	list(APPEND HEADERS_VENDORS ${HEADERS_VENDORS_CUDA_HELPER})
endif()
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/../shaders" FILES ${SHADERS})
target_sources(vtx_renderer
	PRIVATE ${SOURCES}
	PRIVATE ${SOURCES_VENDORS}
	PRIVATE ${SHADERS}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include/public" FILES ${HEADERS_PUBLIC}
	PRIVATE FILE_SET private_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include/private" FILES ${HEADERS_PRIVATE}
	PRIVATE FILE_SET vendors_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../vendor" FILES ${HEADERS_VENDORS}
)

# OpenGL.
if(WIN32)
	target_link_libraries(vtx_renderer PRIVATE opengl32)
elseif(LINUX)
	find_package(X11 REQUIRED)
	find_package(wayland REQUIRED)
	target_link_libraries(vtx_renderer PRIVATE OpenGL::OpenGL OpenGL::EGL X11::X11 wayland::wayland-client)
endif()

# Cuda.
if(VTX_CUDA_ENABLED)
	target_link_libraries(vtx_renderer PRIVATE CUDA::toolkit)

	set_target_properties(vtx_renderer PROPERTIES
		CUDA_ARCHITECTURES "${VTX_CUDA_ARCH}"
		CUDA_SEPARABLE_COMPILATION ON
		CUDA_RESOLVE_DEVICE_SYMBOLS ON
		CUDA_STANDARD 20
	)
	target_compile_options(vtx_renderer PRIVATE 
		$<$<AND:$<COMPILE_LANGUAGE:CUDA>,$<CONFIG:Debug>>:
			--generate-line-info
		>
		$<$<COMPILE_LANGUAGE:CUDA>:
			--use_fast_math
			--relocatable-device-code=true
			--extended-lambda
			-Xcompiler=/Zc:preprocessor
			-Xcudafe
			--diag_suppress=esa_on_defaulted_function_ignored
			-Wno-deprecated-gpu-targets
		>
	)
	target_compile_definitions(vtx_renderer PRIVATE VTX_CUDA_ENABLED)
endif()

# Tests.
file(GLOB_RECURSE TESTS "${CMAKE_CURRENT_LIST_DIR}/../test/*")
add_executable(vtx_renderer_test ${TESTS})
set_property(TARGET vtx_renderer_test PROPERTY FOLDER "test")
vtx_configure_target(vtx_renderer_test)

target_link_libraries(vtx_renderer PUBLIC vtx_util::vtx_util)
target_link_libraries(vtx_renderer PUBLIC vtx_core::vtx_core)
target_link_libraries(vtx_renderer_test PRIVATE vtx_util::vtx_util)
target_link_libraries(vtx_renderer_test PRIVATE vtx_core::vtx_core)

target_link_libraries(vtx_renderer_test PRIVATE vtx_renderer::vtx_renderer)
target_link_libraries(vtx_renderer_test PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(vtx_renderer_test DISCOVERY_MODE PRE_TEST)
