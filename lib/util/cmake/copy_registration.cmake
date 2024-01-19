function(register_build_directory_copy dir)
	# We register the copy dir in a not-so-hidden property that will be used later on
	set_property(GLOBAL PROPERTY _REGISTER_DIR_COPY_property "${_REGISTER_DIR_COPY_property};${dir}")

endfunction()

function(copy_directory_data target dest)

	set(num 0)
	set(property_var)
	get_property(property_var GLOBAL PROPERTY _REGISTER_DIR_COPY_property)
	list(POP_FRONT property_var)
	foreach(dir_it IN LISTS  property_var)
		message("Looping on <${dir_it}>")
		add_custom_target("${target}_directory_copy${num}" ALL COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${dir_it} $<TARGET_FILE_DIR:${target}>/lol )
		set(num  (num + 1))
	endforeach()
endfunction()