set(_VTX_CORE_COPY_DATA_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

function(vtx_core_copy_data target)
	vtx_copy_directory(
		${target}
		"${_VTX_CORE_COPY_DATA_ROOT}/data"
		"$<TARGET_FILE_DIR:${target}>/data"
	)
endfunction()
