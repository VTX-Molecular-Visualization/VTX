set(_VTX_PYTHON_BINDING_COPY_RUNTIME_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

function(_vtx_python_binding_append_runtime_root roots_var candidate)
	if(IS_DIRECTORY "${candidate}")
		get_filename_component(candidate_realpath "${candidate}" REALPATH)
		list(APPEND ${roots_var} "${candidate_realpath}")
		set(${roots_var} "${${roots_var}}" PARENT_SCOPE)
	endif()
endfunction()

function(_vtx_python_binding_add_python_script_target target source_dir)
	set(copy_target "vtx_python_binding_copy_python_script_${target}")
	set(copy_stamp "${CMAKE_CURRENT_BINARY_DIR}/${copy_target}.stamp")
	set(destination_dir "$<TARGET_FILE_DIR:${target}>/python_script")

	file(GLOB_RECURSE python_script_files CONFIGURE_DEPENDS "${source_dir}/*")

	add_custom_command(
		OUTPUT "${copy_stamp}"
		COMMAND ${CMAKE_COMMAND} -E make_directory "${destination_dir}"
		COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
			"${source_dir}"
			"${destination_dir}"
		COMMAND ${CMAKE_COMMAND} -E touch "${copy_stamp}"
		DEPENDS ${python_script_files}
		COMMENT "Syncing python_script for ${target}"
		VERBATIM
	)

	add_custom_target(${copy_target} DEPENDS "${copy_stamp}")
	add_dependencies(${target} ${copy_target})
endfunction()

function(vtx_python_binding_copy_runtime target)
	set(_vtx_python_binding_runtime_roots)
	_vtx_python_binding_append_runtime_root(_vtx_python_binding_runtime_roots "${PROJECT_SOURCE_DIR}")
	_vtx_python_binding_append_runtime_root(_vtx_python_binding_runtime_roots "${CMAKE_CURRENT_SOURCE_DIR}")
	_vtx_python_binding_append_runtime_root(_vtx_python_binding_runtime_roots "${_VTX_PYTHON_BINDING_COPY_RUNTIME_ROOT}")
	if(DEFINED CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE STREQUAL "")
		_vtx_python_binding_append_runtime_root(
			_vtx_python_binding_runtime_roots
			"${_VTX_PYTHON_BINDING_COPY_RUNTIME_ROOT}/build/${CMAKE_BUILD_TYPE}"
		)
	endif()
	_vtx_python_binding_append_runtime_root(_vtx_python_binding_runtime_roots "${_VTX_PYTHON_BINDING_COPY_RUNTIME_ROOT}/build")

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
			foreach(_vtx_python_binding_runtime_dll IN LISTS _vtx_python_binding_runtime_dlls)
				vtx_copy_file(
					${target}
					"${_vtx_python_binding_runtime_dll}"
					"$<TARGET_FILE_DIR:${target}>"
				)
			endforeach()
		endif()
	endforeach()
endfunction()
