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
	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		target_compile_definitions(p_target PRIVATE _DEBUG)
	endif()
endfunction()