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
	# The first of the list is always empty, since the first call of vtx_register_build_directory_copy place a ; up front. So we need to get rid of that empty value on both lists
	list(POP_FRONT property_var_dir_src)
	list(POP_FRONT property_var_dir_dest)

	set(_already_registered_src)
	set(_already_registered_dest)

	foreach(src_it IN LISTS  property_var_dir_src)
		# get the destination associated with the source
		list(GET property_var_dir_dest ${num} current_dest)
		set(_COPY_TARGET_NAME "z_${target}_directory_copy_${num}")
		MATH(EXPR num "${num}+1")

		# Now we need to check if we already registered the same src/dest couple so we can skip it if we do.
		list(FIND _already_registered_src ${src_it} _already_registered_src_idx)
		list(FIND _already_registered_dest ${current_dest} _already_registered_dest_idx)
		
		if(_already_registered_src_idx EQUAL _already_registered_dest_idx AND ( NOT _already_registered_dest_idx EQUAL "-1"))
			message("VTX -- Not registering <${src_it}> into <${current_dest}> directory copy : src/dest couple already registered.")
			continue()
		endif()
		
		# Now, we know the src/dest couple has never been registered before. So we add it to the list of seen src and dest and then we can add the target
		list(APPEND _already_registered_src ${src_it})
		list(APPEND _already_registered_dest ${current_dest})
		
		message("VTX -- Registering ${_COPY_TARGET_NAME} as a post build event to copy directory <${src_it}> into <${current_dest}> ")
		add_custom_target("${_COPY_TARGET_NAME}" ALL COMMAND ${CMAKE_COMMAND} -E copy_directory  ${src_it} $<TARGET_FILE_DIR:${target}>/${current_dest})
		
		add_dependencies("${target}" "${_COPY_TARGET_NAME}")

	endforeach()
	
	# We copy files
	set(num 0)
	set(property_var_file_src)
	set(property_var_file_dest)
	get_property(property_var_file_src  GLOBAL PROPERTY _REGISTER_FILE_COPY_SOURCE_property)
	get_property(property_var_file_dest GLOBAL PROPERTY _REGISTER_FILE_COPY_DEST_property)
	# The first of the list is always empty, since the first call of vtx_register_build_file_copy place a ; up front. So we need to get rid of that empty value on both lists
	list(POP_FRONT property_var_file_src)
	list(POP_FRONT property_var_file_dest)

	foreach(src_it IN LISTS  property_var_file_src)
		list(GET property_var_file_dest ${num} current_dest)
		set(_COPY_TARGET_NAME "z_${target}_file_copy_${num}")
		message("VTX -- Registering ${_COPY_TARGET_NAME} as a post build event to copy file <${src_it}> into <${current_dest}> ")
		add_custom_target("${_COPY_TARGET_NAME}" ALL COMMAND ${CMAKE_COMMAND} -E copy ${src_it} $<TARGET_FILE_DIR:${target}>/${current_dest})
		
		add_dependencies("${target}" "${_COPY_TARGET_NAME}")
		
		MATH(EXPR num "${num}+1")

	endforeach()

	message("VTX -- All copy targets have been added.")
endfunction()
	
# All copies are cumulative until this function is called
function(vtx_clear_registered_copies)
	set_property(GLOBAL PROPERTY _REGISTER_DIR_COPY_SOURCE_property "")
	set_property(GLOBAL PROPERTY _REGISTER_DIR_COPY_DEST_property "")
endfunction()
