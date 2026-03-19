set(_VTX_TOOL_MDPREP_COPY_DATA_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

function(_vtx_tool_mdprep_append_data_root roots_var candidate)
	if(IS_DIRECTORY "${candidate}")
		get_filename_component(candidate_realpath "${candidate}" REALPATH)
		list(APPEND ${roots_var} "${candidate_realpath}")
		set(${roots_var} "${${roots_var}}" PARENT_SCOPE)
	endif()
endfunction()

function(vtx_tool_mdprep_copy_data target)
	set(template_destination_dir "$<TARGET_FILE_DIR:${target}>/data/tools/mdprep/gromacs/templates")
	set(copy_target "vtx_tool_mdprep_copy_data_${target}")
	set(copy_stamp "${CMAKE_CURRENT_BINARY_DIR}/${copy_target}.stamp")

	set(_vtx_tool_mdprep_data_roots)
	_vtx_tool_mdprep_append_data_root(_vtx_tool_mdprep_data_roots "${PROJECT_SOURCE_DIR}")
	_vtx_tool_mdprep_append_data_root(_vtx_tool_mdprep_data_roots "${CMAKE_CURRENT_SOURCE_DIR}")
	_vtx_tool_mdprep_append_data_root(_vtx_tool_mdprep_data_roots "${_VTX_TOOL_MDPREP_COPY_DATA_ROOT}")

	unset(template_source_dir)
	foreach(_vtx_tool_mdprep_data_root IN LISTS _vtx_tool_mdprep_data_roots)
		if(IS_DIRECTORY "${_vtx_tool_mdprep_data_root}/data/templates")
			set(template_source_dir "${_vtx_tool_mdprep_data_root}/data/templates")
			break()
		endif()
	endforeach()

	if(NOT DEFINED template_source_dir)
		message(FATAL_ERROR "Unable to locate mdprep templates directory.")
	endif()

	file(GLOB_RECURSE template_files CONFIGURE_DEPENDS "${template_source_dir}/*")

	add_custom_command(
		OUTPUT "${copy_stamp}"
		COMMAND ${CMAKE_COMMAND} -E make_directory "${template_destination_dir}"
		COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
			"${template_source_dir}"
			"${template_destination_dir}"
		COMMAND ${CMAKE_COMMAND} -E touch "${copy_stamp}"
		DEPENDS ${template_files}
		COMMENT "Syncing mdprep templates for ${target}"
		VERBATIM
	)

	add_custom_target(${copy_target} DEPENDS "${copy_stamp}")
	add_dependencies(${target} ${copy_target})
endfunction()
