# Register a directory for later content copy. The destination argument is a path relative to the build dir.
function(register_build_directory_copy src dest)
	# We register the copy dir in a not-so-hidden property that will be used later on
	set_property(GLOBAL PROPERTY _REGISTER_DIR_COPY_SOURCE_property "${_REGISTER_DIR_COPY_SOURCE_property};${src}")
	set_property(GLOBAL PROPERTY _REGISTER_DIR_COPY_DEST_property "${_REGISTER_DIR_COPY_DEST_property};${dest}")

endfunction()

function(copy_directory_data target dest)

	set(num 0)
	set(property_var_src)
	set(property_var_dest)
	get_property(property_var_src GLOBAL PROPERTY _REGISTER_DIR_COPY_SOURCE_property)
	get_property(property_var_dest GLOBAL PROPERTY _REGISTER_DIR_COPY_DEST_property)
	list(POP_FRONT property_var_src)
	list(POP_FRONT property_var_dest)
	foreach(src_it IN LISTS  property_var_src)
		message("Looping on <${src_it}>")
		list(GET property_var_dest ${num} current_dest)
		message("Destination is <${current_dest}>")
		add_custom_target("${target}_directory_copy${num}" ALL COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${src_it} $<TARGET_FILE_DIR:${target}>/${current_dest})
		set(num  (num + 1))
	endforeach()
endfunction()