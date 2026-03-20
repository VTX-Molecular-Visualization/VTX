set(_VTX_RENDERER_COPY_SHADERS_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

function(vtx_renderer_copy_shaders target)
	set(shader_source_dir "${_VTX_RENDERER_COPY_SHADERS_ROOT}/shaders")
	set(shader_destination_dir "$<TARGET_FILE_DIR:${target}>/shaders")
	vtx_add_copy_directory_target(
		${target}
		"vtx_renderer_copy_shaders"
		"${shader_source_dir}"
		"${shader_destination_dir}"
		"Syncing renderer shaders for ${target}"
	)
endfunction()
