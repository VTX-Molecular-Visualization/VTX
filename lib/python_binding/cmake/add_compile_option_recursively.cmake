function(addCompileOptionRecursively p_target p_compileOption)

	get_target_property(TARGETS ${p_target} LINK_LIBRARIES)

	foreach (LIB_TARGET IN LISTS TARGETS)
	
		if (TARGET ${LIB_TARGET})
		
			get_target_property(REAL_TARGET ${LIB_TARGET} ALIASED_TARGET)
			
			if (TARGET ${REAL_TARGET})
				set (LIB_TARGET ${REAL_TARGET})
			endif()
				
			set_target_properties(${LIB_TARGET} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
			#target_compile_options(${LIB_TARGET} PRIVATE "-fPIC")
			message("add compile option on ${LIB_TARGET}")
				
			addCompileOptionRecursively(${LIB_TARGET} ${p_compileOption})
		endif()
		
	endforeach()

endfunction()