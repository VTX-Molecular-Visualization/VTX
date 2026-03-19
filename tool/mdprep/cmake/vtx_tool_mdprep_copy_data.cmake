set(_VTX_TOOL_MDPREP_COPY_DATA_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

function(vtx_tool_mdprep_copy_data target)
	set(template_source_dir "${_VTX_TOOL_MDPREP_COPY_DATA_ROOT}/data/templates")
	set(template_destination_dir "$<TARGET_FILE_DIR:${target}>/data/tools/mdprep/gromacs/templates")
	set(copy_target "vtx_tool_mdprep_copy_data_${target}")
	set(copy_stamp "${CMAKE_CURRENT_BINARY_DIR}/${copy_target}.stamp")

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
