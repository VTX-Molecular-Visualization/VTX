function(setTargetPropertyRecursively p_target p_property p_value)

	get_target_property(PROPERTY_STATE ${p_target} ${p_property} )

	if ( NOT(${PROPERTY_STATE}) )

		message("add compile option ${p_property} on ${p_target}")
		set_target_properties(${p_target} PROPERTIES ${p_property} ${p_value})
	
	endif()

	get_target_property(TARGETS ${p_target} LINK_LIBRARIES)

	foreach (LIB_TARGET IN LISTS TARGETS)

		if (TARGET ${LIB_TARGET})
			
			get_target_property(REAL_TARGET ${LIB_TARGET} ALIASED_TARGET)
			
			if (TARGET ${REAL_TARGET})
				set (LIB_TARGET ${REAL_TARGET})
			endif()
				
			setTargetPropertyRecursively(${LIB_TARGET} ${p_property} ${p_value})

		endif()
		
	endforeach()

endfunction()
