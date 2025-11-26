include("${CMAKE_CURRENT_LIST_DIR}/vtx_link_cuda.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/vtx_renderer_copy_files.cmake")

# Lib.
add_library(vtx_renderer)
vtx_configure_target(vtx_renderer)

file(GLOB_RECURSE HEADERS_PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../include/public/*")
file(GLOB_RECURSE HEADERS_PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../include/private/*")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")
file(GLOB_RECURSE HEADERS_VENDORS "${CMAKE_CURRENT_LIST_DIR}/../vendor/*.h")
file(GLOB_RECURSE SOURCES_VENDORS "${CMAKE_CURRENT_LIST_DIR}/../vendor/*.c")
file(GLOB_RECURSE SHADERS "${CMAKE_CURRENT_LIST_DIR}/../shaders/*")
target_sources(vtx_renderer
	PRIVATE ${SOURCES}
	PRIVATE ${SOURCES_VENDORS}
	PRIVATE ${SHADERS}
	PUBLIC FILE_SET public_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include/public" FILES ${HEADERS_PUBLIC}
	PRIVATE FILE_SET private_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include/private" FILES ${HEADERS_PRIVATE}
	PRIVATE FILE_SET vendors_headers TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../vendor" FILES ${HEADERS_VENDORS}
)

# Cuda.
include(CheckLanguage)
check_language(CUDA)
if (CMAKE_CUDA_COMPILER)
	enable_language(CUDA)
	find_package(CUDAToolkit)
	target_link_libraries(vtx_renderer PRIVATE CUDA::toolkit)

	# Check if the CUDA architecture is defined in the Conan options, or use a default value.
	if (NOT DEFINED VTX_CUDA_ARCH)
		set(VTX_CUDA_ARCH "native")
	endif()

	set_target_properties(vtx_renderer PROPERTIES
		#https://en.wikipedia.org/wiki/CUDA
		#CUDA_ARCHITECTURES "50;52;60;61;70;75;80;86;89;90"
		#CUDA_ARCHITECTURES "120-real" 
		CUDA_ARCHITECTURES ${VTX_CUDA_ARCH}
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
			-Xcudafe
			--diag_suppress=esa_on_defaulted_function_ignored
			-Wno-deprecated-gpu-targets
		>
	)
	target_compile_definitions(vtx_renderer PRIVATE VTX_CUDA_ENABLED)
else()
	message(STATUS "CUDA not found")
endif()

# Tests.
file(GLOB_RECURSE TESTS "${CMAKE_CURRENT_LIST_DIR}/../test/*")
add_executable(vtx_renderer_test ${TESTS})
vtx_configure_target(vtx_renderer_test)

if (NOT DEFINED _VTX_RENDERER_CONAN)
	target_link_libraries(vtx_renderer PRIVATE vtx_util)
	target_link_libraries(vtx_renderer PRIVATE vtx_core)
	target_link_libraries(vtx_renderer_test PRIVATE vtx_util)
else()
	target_link_libraries(vtx_renderer PRIVATE vtx_util::vtx_util)
	target_link_libraries(vtx_renderer PRIVATE vtx_core::vtx_core)
	target_link_libraries(vtx_renderer_test PRIVATE vtx_util::vtx_util)
endif()

target_link_libraries(vtx_renderer_test PRIVATE vtx_renderer)
target_link_libraries(vtx_renderer_test PRIVATE vtx_core)
target_link_libraries(vtx_renderer_test PRIVATE Catch2::Catch2WithMain)

vtx_copy_registered_data(vtx_renderer) # allow declared files to be copied on build

include(CTest)
include(Catch)
catch_discover_tests(vtx_renderer_test DISCOVERY_MODE PRE_TEST)