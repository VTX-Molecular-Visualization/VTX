option(VTX_ENABLE_NATIVE_OPTIMIZATIONS "Enable CPU-specific release optimizations for local builds." OFF)
option(VTX_ENABLE_INTERPROCEDURAL_OPTIMIZATION "Enable interprocedural optimization for VTX targets." OFF)
option(VTX_ENABLE_CLANG_TIDY "Enable clang-tidy analysis for VTX targets." ON)

# Exclude vendor sources from linting.
function(_vtx_exclude_vendor_sources_from_linting p_target)
	get_target_property(srcs ${p_target} SOURCES)
	foreach(src IN LISTS srcs)
		file(TO_CMAKE_PATH "${src}" src_patch)
		if(src_patch MATCHES "(^|/)vendor/")
			set_source_files_properties("${src}" TARGET_DIRECTORY ${p_target} PROPERTIES SKIP_LINTING ON)
		endif()
	endforeach()
endfunction()

# Configure a target with VTX-specific settings.
function(vtx_configure_target p_target)
	# Static analysis.
	if(VTX_ENABLE_CLANG_TIDY)
		get_filename_component(_vtx_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
		# TODO: remove hardcoded path.
		find_program(VTX_CLANG_TIDY_EXECUTABLE NAMES clang-tidy
			HINTS "${_vtx_compiler_dir}/../../../../../Llvm/x64/bin"
			REQUIRED
		)
		set_property(TARGET ${p_target} PROPERTY CXX_CLANG_TIDY "${VTX_CLANG_TIDY_EXECUTABLE}")
		if(MSVC)
			set_property(TARGET ${p_target} APPEND PROPERTY CXX_CLANG_TIDY "--extra-arg=/EHsc")
		endif()
		# Defer function call to the end of the configuration to ensure that all sources have been added to the target.
		cmake_language(EVAL CODE "cmake_language(DEFER CALL _vtx_exclude_vendor_sources_from_linting [[${p_target}]])"
		)
	endif()

	# Compiler-specific options.
	if(APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
		target_compile_options(${p_target} PRIVATE
			$<$<COMPILE_LANGUAGE:CXX>:-fexperimental-library>
		)
	elseif(CMAKE_COMPILER_IS_GNUCC)
		target_compile_options(${p_target} PRIVATE -Wpedantic -Wall)
		target_compile_options(${p_target} PRIVATE
        	$<$<CONFIG:Release>:-O2 -ffast-math>
    	)
		if(VTX_ENABLE_NATIVE_OPTIMIZATIONS)
			target_compile_options(${p_target} PRIVATE
				$<$<CONFIG:Release>:-march=native>
			)
		endif()
	elseif(MSVC)
		# General.
		target_compile_options(${p_target} PRIVATE 
			$<$<COMPILE_LANGUAGE:CXX>:/W3>              # Warning level 3
			$<$<COMPILE_LANGUAGE:CXX>:/WX>              # Warnings as errors
			$<$<COMPILE_LANGUAGE:CXX>:/MP>		        # Multicore compilation.
			$<$<COMPILE_LANGUAGE:CXX>:/sdl>             # Security Checks
			$<$<COMPILE_LANGUAGE:CXX>:/utf-8>           # UTF-8 encoding
			$<$<COMPILE_LANGUAGE:CXX>:/fp:fast>         # Floating Point
			$<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>  # Report the correct C++ standard in __cplusplus
			$<$<COMPILE_LANGUAGE:CXX>:/Zc:preprocessor> # Use the standard-conforming preprocessor
		)
		target_compile_definitions(${p_target} PRIVATE
			NOMINMAX		    # Prevent Windows headers from defining min/max macros
			WIN32_LEAN_AND_MEAN # Exclude rarely used Windows headers
		)
		# Optimization.
		target_compile_options(${p_target} PRIVATE 
			$<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:/O2 /Ob2 /Ot /Oi>
		)
		if(VTX_ENABLE_NATIVE_OPTIMIZATIONS)
			target_compile_options(${p_target} PRIVATE
				$<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:/arch:AVX2>
			)
		endif()
	endif()
	
	if(VTX_ENABLE_INTERPROCEDURAL_OPTIMIZATION)
		set_property(TARGET ${p_target} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	endif()
	
	# Force _DEBUG preprocessor on all plateforms.
	if(DEFINED CMAKE_BUILD_TYPE)
		if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
			target_compile_definitions(${p_target} PRIVATE _DEBUG)
		endif()
	endif()
endfunction()
