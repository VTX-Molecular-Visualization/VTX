set(_VTX_PYTHON_BINDING_COPY_RUNTIME_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

# Create a dedicated stamped target so python scripts are synced once per runtime target.
function(_vtx_python_binding_add_python_script_target target source_dir)
	set(destination_dir "$<TARGET_FILE_DIR:${target}>/python_script")
	vtx_add_copy_directory_target(
		${target}
		"vtx_python_binding_copy_python_script"
		"${source_dir}"
		"${destination_dir}"
		"Syncing python_script for ${target}"
	)
endfunction()

# Bundle python scripts, embedded runtime files and Windows Python DLLs for a target.
function(vtx_python_binding_copy_runtime target)
	set(_vtx_python_binding_runtime_roots)
	vtx_append_existing_realpath(_vtx_python_binding_runtime_roots "${PROJECT_SOURCE_DIR}")
	vtx_append_existing_realpath(_vtx_python_binding_runtime_roots "${CMAKE_CURRENT_SOURCE_DIR}")
	vtx_append_existing_realpath(_vtx_python_binding_runtime_roots "${_VTX_PYTHON_BINDING_COPY_RUNTIME_ROOT}")
	if(DEFINED CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE STREQUAL "")
		vtx_append_existing_realpath(
			_vtx_python_binding_runtime_roots
			"${_VTX_PYTHON_BINDING_COPY_RUNTIME_ROOT}/build/${CMAKE_BUILD_TYPE}"
		)
	endif()
	vtx_append_existing_realpath(_vtx_python_binding_runtime_roots "${_VTX_PYTHON_BINDING_COPY_RUNTIME_ROOT}/build")

	set(_vtx_python_binding_python_script_target_added FALSE)
	foreach(_vtx_python_binding_runtime_root IN LISTS _vtx_python_binding_runtime_roots)
		if(
			NOT _vtx_python_binding_python_script_target_added
			AND EXISTS "${_vtx_python_binding_runtime_root}/python_script"
		)
			_vtx_python_binding_add_python_script_target(
				${target}
				"${_vtx_python_binding_runtime_root}/python_script"
			)
			set(_vtx_python_binding_python_script_target_added TRUE)
		endif()

		if(EXISTS "${_vtx_python_binding_runtime_root}/external/python")
			vtx_copy_directory(
				${target}
				"${_vtx_python_binding_runtime_root}/external/python"
				"$<TARGET_FILE_DIR:${target}>/external/python"
			)
		endif()

		if(WIN32)
			file(GLOB _vtx_python_binding_runtime_dlls "${_vtx_python_binding_runtime_root}/python*.dll")
			vtx_copy_files(${target} "$<TARGET_FILE_DIR:${target}>" ${_vtx_python_binding_runtime_dlls})
		endif()
	endforeach()
endfunction()
