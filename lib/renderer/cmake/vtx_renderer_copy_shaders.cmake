set(_VTX_RENDERER_COPY_SHADERS_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

function(vtx_renderer_copy_shaders target)
	set(shader_source_dir "${_VTX_RENDERER_COPY_SHADERS_ROOT}/shaders")
	set(shader_destination_dir "$<TARGET_FILE_DIR:${target}>/shaders")
	set(copy_target "vtx_renderer_copy_shaders_${target}")
	set(copy_stamp "${CMAKE_CURRENT_BINARY_DIR}/${copy_target}.stamp")

	file(GLOB_RECURSE shader_files CONFIGURE_DEPENDS "${shader_source_dir}/*")

	add_custom_command(
		OUTPUT "${copy_stamp}"
		COMMAND ${CMAKE_COMMAND} -E make_directory "${shader_destination_dir}"
		COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
			"${shader_source_dir}"
			"${shader_destination_dir}"
		COMMAND ${CMAKE_COMMAND} -E touch "${copy_stamp}"
		DEPENDS ${shader_files}
		COMMENT "Syncing renderer shaders for ${target}"
		VERBATIM
	)

	add_custom_target(${copy_target} DEPENDS "${copy_stamp}")
	add_dependencies(${target} ${copy_target})
endfunction()
