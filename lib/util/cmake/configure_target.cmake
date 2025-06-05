function(configure_target p_target)
	if(CMAKE_COMPILER_IS_GNUCC)
		target_compile_options(${p_target} PRIVATE -Wpedantic -Wall)
	elseif(MSVC)
		# General.
		target_compile_options(${p_target} PRIVATE 
			$<$<COMPILE_LANGUAGE:CXX>:/W3>            # Warning level 3
			$<$<COMPILE_LANGUAGE:CXX>:/WX>            # Warnings as errors
			$<$<COMPILE_LANGUAGE:CXX>:/MP>		      # Multicore compilation.
			$<$<COMPILE_LANGUAGE:CXX>:/sdl>           # Security Checks
			$<$<COMPILE_LANGUAGE:CXX>:/utf-8>         # UTF-8 encoding
			$<$<COMPILE_LANGUAGE:CXX>:/fp:fast>       # Floating Point
			$<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>
		)
		# Optimization.
		target_compile_options(${p_target} PRIVATE 
			$<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:/O2 /Ob2 /Ot /Oi>
		)
	endif()
	# Force _DEBUG preprocessor on all plateforms.
	if(DEFINED CMAKE_BUILD_TYPE)
		if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
			target_compile_definitions(${p_target} PRIVATE _DEBUG)
		endif()
	endif()	
endfunction()