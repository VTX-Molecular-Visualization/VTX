option(VTX_ENABLE_NATIVE_OPTIMIZATIONS "Enable CPU-specific release optimizations for local builds." OFF)

function(vtx_configure_target p_target)
	if(CMAKE_COMPILER_IS_GNUCC)
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
	
	set_property(TARGET ${p_target} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	
	# Force _DEBUG preprocessor on all plateforms.
	if(DEFINED CMAKE_BUILD_TYPE)
		if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
			target_compile_definitions(${p_target} PRIVATE _DEBUG)
		endif()
	endif()
endfunction()
