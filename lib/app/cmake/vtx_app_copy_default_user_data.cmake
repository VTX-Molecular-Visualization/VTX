set(_VTX_APP_COPY_DEFAULT_USER_DATA_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

function(vtx_app_copy_default_user_data target)
	vtx_add_copy_directory_target(
		${target}
		"vtx_app_copy_default_user_data"
		"${_VTX_APP_COPY_DEFAULT_USER_DATA_ROOT}/default_user_data"
		"$<TARGET_FILE_DIR:${target}>/default_user_data"
		"Syncing default user data for ${target}"
	)
endfunction()
