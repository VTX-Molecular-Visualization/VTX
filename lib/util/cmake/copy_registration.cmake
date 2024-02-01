# Register a directory for later content copy. The destination argument is a path relative to the build dir.
function(vtx_register_build_directory_copy src dest)
	# We register the copy dir in a not-so-hidden property that will be used later on
	# It is important that both properties are being kept symetrical
	get_property(property_var_src GLOBAL PROPERTY _REGISTER_DIR_COPY_SOURCE_property)
	get_property(property_var_dest GLOBAL PROPERTY _REGISTER_DIR_COPY_DEST_property)
	set_property(GLOBAL PROPERTY _REGISTER_DIR_COPY_SOURCE_property "${property_var_src};${src}")
	set_property(GLOBAL PROPERTY _REGISTER_DIR_COPY_DEST_property "${property_var_dest};${dest}")
endfunction()

# Add on-build copy for all registered directories into build folder
function(vtx_copy_directory_data target dest)

	set(num 0)
	set(property_var_src)
	set(property_var_dest)
	get_property(property_var_src GLOBAL PROPERTY _REGISTER_DIR_COPY_SOURCE_property)
	get_property(property_var_dest GLOBAL PROPERTY _REGISTER_DIR_COPY_DEST_property)
	# The first of the list is always empty, since the first call of register_build_directory_copy place a ; up front. So we need to get rid of that empty value on both lists
	list(POP_FRONT property_var_src)
	list(POP_FRONT property_var_dest)

	foreach(src_it IN LISTS  property_var_src)
		list(GET property_var_dest ${num} current_dest)
		message("VTX -- Registering ${target}_directory_copy${num} as a post build event to copy directory <${src_it}> into <${current_dest}> ")
		add_custom_target("${target}_directory_copy${num}" ALL COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${src_it} $<TARGET_FILE_DIR:${target}>/${current_dest})
		
		MATH(EXPR num "${num}+1")

	endforeach()

	# Since the post-build event have been subscribed, we can clean global properties to avoid mishaps
	set_property(GLOBAL PROPERTY _REGISTER_DIR_COPY_SOURCE_property "")
	set_property(GLOBAL PROPERTY _REGISTER_DIR_COPY_DEST_property "")
endfunction()