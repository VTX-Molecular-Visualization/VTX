set(_VTX_TOOL_MDPREP_COPY_DATA_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

# Copy mdprep templates to the runtime layout expected by the tool.
function(vtx_tool_mdprep_copy_data target)
	set(template_source_dir "${_VTX_TOOL_MDPREP_COPY_DATA_ROOT}/data/templates")
	set(template_destination_dir "$<TARGET_FILE_DIR:${target}>/data/tools/mdprep/gromacs/templates")

	if(NOT IS_DIRECTORY "${template_source_dir}")
		message(FATAL_ERROR "Unable to locate mdprep templates under <${_VTX_TOOL_MDPREP_COPY_DATA_ROOT}>.")
	endif()

	vtx_add_copy_directory_target(
		${target}
		"vtx_tool_mdprep_copy_data"
		"${template_source_dir}"
		"${template_destination_dir}"
		"Syncing mdprep templates for ${target}"
	)
endfunction()

# Bundle Gromacs runtime files prepared by the mdprep Conan package for a target.
function(vtx_tool_mdprep_copy_runtime target)
	if(NOT DEFINED VTX_TOOL_MDPREP_RUNTIME_ROOT OR VTX_TOOL_MDPREP_RUNTIME_ROOT STREQUAL "")
		message(FATAL_ERROR "VTX_TOOL_MDPREP_RUNTIME_ROOT must be defined before calling vtx_tool_mdprep_copy_runtime().")
	endif()

	set(gromacs_tools_source_dir "${VTX_TOOL_MDPREP_RUNTIME_ROOT}/external/tools")
	set(gromacs_top_source_dir "${VTX_TOOL_MDPREP_RUNTIME_ROOT}/data/tools/mdprep/gromacs/top")

	if(NOT IS_DIRECTORY "${gromacs_tools_source_dir}")
		message(FATAL_ERROR "Unable to locate mdprep Gromacs runtime under <${VTX_TOOL_MDPREP_RUNTIME_ROOT}>.")
	endif()
	if(NOT IS_DIRECTORY "${gromacs_top_source_dir}")
		message(FATAL_ERROR "Unable to locate mdprep Gromacs topology data under <${VTX_TOOL_MDPREP_RUNTIME_ROOT}>.")
	endif()

	vtx_copy_directory(
		${target}
		"${gromacs_tools_source_dir}"
		"$<TARGET_FILE_DIR:${target}>/external/tools"
	)
	vtx_copy_directory(
		${target}
		"${gromacs_top_source_dir}"
		"$<TARGET_FILE_DIR:${target}>/data/tools/mdprep/gromacs/top"
	)
endfunction()
