# Register a directory for later content copy. The destination argument is a path relative to the build dir.
function(vtx_register_build_directory_copy src dest)
	# We register the copy dir in a not-so-hidden property that will be used later on
	# It is important that both properties are being kept symetrical
	get_property(property_var_dir_src GLOBAL PROPERTY _REGISTER_DIR_COPY_SOURCE_property)
	get_property(property_var_dir_dest GLOBAL PROPERTY _REGISTER_DIR_COPY_DEST_property)
	set_property(GLOBAL PROPERTY _REGISTER_DIR_COPY_SOURCE_property "${property_var_dir_src};${src}")
	set_property(GLOBAL PROPERTY _REGISTER_DIR_COPY_DEST_property "${property_var_dir_dest};${dest}")
endfunction()

# Register a file for later copy. The destination argument is a path relative to the build dir.
# If no file name is given into the dest relative path, the initial file name will be used.
# If a filename is providen, it will be rename at copy destination.
function(vtx_register_build_file_copy src dest)
	# We register the copy dir in a not-so-hidden property that will be used later on
	# It is important that both properties are being kept symetrical
	get_property(property_var_file_src GLOBAL PROPERTY _REGISTER_FILE_COPY_SOURCE_property)
	get_property(property_var_file_dest GLOBAL PROPERTY _REGISTER_FILE_COPY_DEST_property)
	set_property(GLOBAL PROPERTY _REGISTER_FILE_COPY_SOURCE_property "${property_var_file_src};${src}")
	set_property(GLOBAL PROPERTY _REGISTER_FILE_COPY_DEST_property   "${property_var_file_dest};${dest}")
endfunction()

# Add on-build copy for all registered directories into build folder
function(vtx_copy_registered_data target)

	# We copy directories
	set(num 0)
	set(property_var_dir_src)
	set(property_var_dir_dest)
	get_property(property_var_dir_src GLOBAL PROPERTY _REGISTER_DIR_COPY_SOURCE_property)
	get_property(property_var_dir_dest GLOBAL PROPERTY _REGISTER_DIR_COPY_DEST_property)
	# The first of the list is always empty, since the first call of register_build_directory_copy place a ; up front. So we need to get rid of that empty value on both lists
	list(POP_FRONT property_var_dir_src)
	list(POP_FRONT property_var_dir_dest)

	foreach(src_it IN LISTS  property_var_dir_src)
		list(GET property_var_dir_dest ${num} current_dest)
		message("VTX -- Registering ${target}_directory_copy${num} as a post build event to copy directory <${src_it}> into <${current_dest}> ")
		add_custom_target("${target}_directory_copy${num}" ALL COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${src_it} $<TARGET_FILE_DIR:${target}>/${current_dest})
		
		MATH(EXPR num "${num}+1")

	endforeach()

	# We copy files
	set(num 0)
	set(property_var_file_src)
	set(property_var_file_dest)
	get_property(property_var_file_src  GLOBAL PROPERTY _REGISTER_FILE_COPY_SOURCE_property)
	get_property(property_var_file_dest GLOBAL PROPERTY _REGISTER_FILE_COPY_DEST_property)
	# The first of the list is always empty, since the first call of register_build_directory_copy place a ; up front. So we need to get rid of that empty value on both lists
	list(POP_FRONT property_var_file_src)
	list(POP_FRONT property_var_file_dest)

	foreach(src_it IN LISTS  property_var_file_src)
		list(GET property_var_file_dest ${num} current_dest)
		message("VTX -- Registering ${target}_file_copy${num} as a post build event to copy file <${src_it}> into <${current_dest}> ")
		add_custom_target("${target}_file_copy${num}" ALL COMMAND ${CMAKE_COMMAND} -E copy ${src_it} $<TARGET_FILE_DIR:${target}>/${current_dest})
		
		MATH(EXPR num "${num}+1")

	endforeach()

endfunction()