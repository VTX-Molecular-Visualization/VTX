# Append an existing path to a caller-owned list after resolving it to a stable real path.
function(vtx_append_existing_realpath out_var candidate)
	if(IS_DIRECTORY "${candidate}" OR EXISTS "${candidate}")
		get_filename_component(candidate_realpath "${candidate}" REALPATH)
		list(APPEND ${out_var} "${candidate_realpath}")
		list(REMOVE_DUPLICATES ${out_var})
		set(${out_var} "${${out_var}}" PARENT_SCOPE)
	endif()
endfunction()

# Generate a stable unique target suffix so repeated runtime copies do not collide.
function(_vtx_next_copy_target_name out_var prefix target)
	get_property(copy_target_index GLOBAL PROPERTY VTX_COPY_TARGET_INDEX)
	if(NOT copy_target_index)
		set(copy_target_index 0)
	endif()

	math(EXPR copy_target_index "${copy_target_index} + 1")
	set_property(GLOBAL PROPERTY VTX_COPY_TARGET_INDEX "${copy_target_index}")
	set(${out_var} "${prefix}_${target}_${copy_target_index}" PARENT_SCOPE)
endfunction()

# Create a stamped copy target so a runtime directory is resynced when its contents change.
function(vtx_add_copy_directory_target target copy_name source_dir destination_dir comment)
	set(copy_target "${copy_name}_${target}")
	set(copy_stamp "${CMAKE_CURRENT_BINARY_DIR}/${copy_target}.stamp")

	file(GLOB_RECURSE copy_source_files CONFIGURE_DEPENDS "${source_dir}/*")

	add_custom_command(
		OUTPUT "${copy_stamp}"
		COMMAND ${CMAKE_COMMAND} -E make_directory "${destination_dir}"
		COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
			"${source_dir}"
			"${destination_dir}"
		COMMAND ${CMAKE_COMMAND} -E touch "${copy_stamp}"
		DEPENDS "${source_dir}" ${copy_source_files}
		COMMENT "${comment}"
		VERBATIM
	)

	add_custom_target(${copy_target} DEPENDS "${copy_stamp}")
	set_property(TARGET ${copy_target} PROPERTY FOLDER "copy")
	add_dependencies(${target} ${copy_target})
endfunction()

# Create a stamped copy target so one or more runtime files are resynced when they change.
function(vtx_add_copy_files_target target copy_name destination_dir comment)
	if(NOT ARGN)
		return()
	endif()

	set(copy_target "${copy_name}_${target}")
	set(copy_stamp "${CMAKE_CURRENT_BINARY_DIR}/${copy_target}.stamp")
	set(copy_commands COMMAND ${CMAKE_COMMAND} -E make_directory "${destination_dir}")

	foreach(copy_source_file IN LISTS ARGN)
		list(APPEND copy_commands
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
				"${copy_source_file}"
				"${destination_dir}"
		)
	endforeach()

	list(APPEND copy_commands COMMAND ${CMAKE_COMMAND} -E touch "${copy_stamp}")

	add_custom_command(
		OUTPUT "${copy_stamp}"
		${copy_commands}
		DEPENDS ${ARGN}
		COMMENT "${comment}"
		VERBATIM
	)

	add_custom_target(${copy_target} DEPENDS "${copy_stamp}")
	set_property(TARGET ${copy_target} PROPERTY FOLDER "copy")
	add_dependencies(${target} ${copy_target})
endfunction()

# Copy a small set of runtime files with incremental rebuild tracking.
function(vtx_copy_files target destination)
	if(NOT ARGN)
		return()
	endif()

	_vtx_next_copy_target_name(copy_name "vtx_copy_files" "${target}")
	vtx_add_copy_files_target(
		${target}
		"${copy_name}"
		"${destination}"
		"Syncing runtime files for ${target}"
		${ARGN}
	)
endfunction()

# Copy a single runtime file with incremental rebuild tracking.
function(vtx_copy_file target source destination)
	vtx_copy_files(${target} "${destination}" "${source}")
endfunction()

# Copy a runtime directory with incremental rebuild tracking.
function(vtx_copy_directory target source destination)
	_vtx_next_copy_target_name(copy_name "vtx_copy_directory" "${target}")
	vtx_add_copy_directory_target(
		${target}
		"${copy_name}"
		"${source}"
		"${destination}"
		"Syncing runtime directory for ${target}"
	)
endfunction()

# Install a runtime directory from a target output tree.
function(vtx_install_target_directory target relative_path)
	set(options OPTIONAL USE_SOURCE_PERMISSIONS)
	set(oneValueArgs DESTINATION)
	cmake_parse_arguments(VTX_INSTALL_DIR "${options}" "${oneValueArgs}" "" ${ARGN})

	if(NOT VTX_INSTALL_DIR_DESTINATION)
		set(VTX_INSTALL_DIR_DESTINATION ".")
	endif()
	if(VTX_INSTALL_DIR_USE_SOURCE_PERMISSIONS)
		set(source_permissions_arg USE_SOURCE_PERMISSIONS)
	else()
		set(source_permissions_arg)
	endif()

	if(VTX_INSTALL_DIR_OPTIONAL)
		install(
			DIRECTORY "$<TARGET_FILE_DIR:${target}>/${relative_path}"
			DESTINATION "${VTX_INSTALL_DIR_DESTINATION}"
			OPTIONAL
			${source_permissions_arg}
		)
	else()
		install(
			DIRECTORY "$<TARGET_FILE_DIR:${target}>/${relative_path}"
			DESTINATION "${VTX_INSTALL_DIR_DESTINATION}"
			${source_permissions_arg}
		)
	endif()
endfunction()

# Install runtime files from a target output tree using glob-like patterns.
function(vtx_install_target_files target)
	set(options OPTIONAL)
	set(oneValueArgs DESTINATION)
	set(multiValueArgs PATTERNS)
	cmake_parse_arguments(VTX_INSTALL_FILES "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(NOT VTX_INSTALL_FILES_DESTINATION)
		set(VTX_INSTALL_FILES_DESTINATION ".")
	endif()

	if(NOT VTX_INSTALL_FILES_PATTERNS)
		return()
	endif()

	if(VTX_INSTALL_FILES_OPTIONAL)
		install(
			DIRECTORY "$<TARGET_FILE_DIR:${target}>/"
			DESTINATION "${VTX_INSTALL_FILES_DESTINATION}"
			OPTIONAL
			FILES_MATCHING
			${VTX_INSTALL_FILES_PATTERNS}
		)
	else()
		install(
			DIRECTORY "$<TARGET_FILE_DIR:${target}>/"
			DESTINATION "${VTX_INSTALL_FILES_DESTINATION}"
			FILES_MATCHING
			${VTX_INSTALL_FILES_PATTERNS}
		)
	endif()
endfunction()

# Install a source/package directory without repeating the current-source-root plumbing.
function(vtx_install_source_directory relative_path)
	set(options OPTIONAL)
	set(oneValueArgs DESTINATION)
	cmake_parse_arguments(VTX_INSTALL_SOURCE "${options}" "${oneValueArgs}" "" ${ARGN})

	if(NOT VTX_INSTALL_SOURCE_DESTINATION)
		set(VTX_INSTALL_SOURCE_DESTINATION ".")
	endif()

	set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/${relative_path}")

	if(VTX_INSTALL_SOURCE_OPTIONAL)
		install(DIRECTORY "${source_dir}" DESTINATION "${VTX_INSTALL_SOURCE_DESTINATION}" OPTIONAL)
	else()
		install(DIRECTORY "${source_dir}" DESTINATION "${VTX_INSTALL_SOURCE_DESTINATION}")
	endif()
endfunction()

# Install exported CMake build modules for a package.
function(vtx_install_build_modules)
	vtx_install_source_directory("cmake")
endfunction()
