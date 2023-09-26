function(configureTarget p_target)
	# Compiler options.
	target_compile_features(${p_target} PRIVATE cxx_std_20)

	if(CMAKE_COMPILER_IS_GNUCC)
		#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
		target_compile_options(${p_target} PRIVATE "-Wpedantic")
		target_compile_options(${p_target} PRIVATE "-Wall")
		#target_compile_options(${p_target} PRIVATE "-Werror")
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
		
		# Preprocessor definitions
		#add_definitions(-D_CRT_SECURE_NO_WARNINGS)
		#(-DNOMINMAX)

	endif()
endfunction()