set(_VTX_CORE_COPY_DATA_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

function(vtx_core_copy_data target)
	vtx_add_copy_directory_target(
		${target}
		"vtx_core_copy_data"
		"${_VTX_CORE_COPY_DATA_ROOT}/data"
		"$<TARGET_FILE_DIR:${target}>/data"
		"Syncing core data for ${target}"
	)
endfunction()
