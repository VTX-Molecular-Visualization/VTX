set(_VTX_TOOL_MDPREP_COPY_DATA_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

# Copy mdprep templates to the runtime layout expected by the tool.
function(vtx_tool_mdprep_copy_data target)
	set(template_destination_dir "$<TARGET_FILE_DIR:${target}>/data/tools/mdprep/gromacs/templates")

	set(_vtx_tool_mdprep_data_roots)
	vtx_append_existing_realpath(_vtx_tool_mdprep_data_roots "${PROJECT_SOURCE_DIR}")
	vtx_append_existing_realpath(_vtx_tool_mdprep_data_roots "${CMAKE_CURRENT_SOURCE_DIR}")
	vtx_append_existing_realpath(_vtx_tool_mdprep_data_roots "${_VTX_TOOL_MDPREP_COPY_DATA_ROOT}")

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

	vtx_add_copy_directory_target(
		${target}
		"vtx_tool_mdprep_copy_data"
		"${template_source_dir}"
		"${template_destination_dir}"
		"Syncing mdprep templates for ${target}"
	)
endfunction()
